#include "mfem.hpp"
#include "pfem_extras.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <set>

//#include "fortran.h"
//#include "lobpcg.h"
#include "temp_multivector.h"

using namespace std;
using namespace mfem;

class ScalarFloquetWaveEquation
{
public:
   ScalarFloquetWaveEquation(ParMesh & pmesh, int order);

   ~ScalarFloquetWaveEquation();

   HYPRE_Int *GetTrueDofOffsets() { return tdof_offsets_; }

   // void SetOmega(double omega) { omega_ = omega; }
   void SetBeta(double beta) { beta_ = beta; }
   void SetAzimuth(double alpha_a) { alpha_a_ = alpha_a; }
   void SetInclination(double alpha_i) { alpha_i_ = alpha_i; }
   void SetSigma(double sigma) { sigma_ = sigma; }

   void SetMassCoef(Coefficient & m) { mCoef_ = &m; }
   void SetStiffnessCoef(Coefficient & k) { kCoef_ = &k; }
   void Setup();

   void PerturbEigenvalues(int nev, double tol,
                           double * lam0, HYPRE_ParVector* v0);

   BlockOperator * GetAOperator() { return A_; }
   BlockOperator * GetBOperator() { return B_; }
   BlockOperator * GetMOperator() { return M_; }
   BlockOperator * GetTOperator() { return T_; }
   BlockOperator * GetPrecond() { return T_; }
   BlockOperator * GetOperator();

   BlockDiagonalPreconditioner * GetBDP() { return BDP_; }

   ParFiniteElementSpace * GetFESpace() { return H1FESpace_; }

private:

   H1_ParFESpace  * H1FESpace_;

   double           alpha_a_;
   double           alpha_i_;
   double           beta_;
   double           sigma_;
   Vector           zeta_;

   Coefficient    * mCoef_;
   Coefficient    * kCoef_;

   Array<int>       block_offsets_;
   Array<int>       block_trueOffsets_;
   Array<HYPRE_Int> tdof_offsets_;

   BlockOperator  * A_;
   BlockOperator  * B_;
   BlockOperator  * M_;
   BlockOperator  * Op_;
   BlockOperator  * T_;

   HypreParMatrix * A0_;
   HypreParMatrix * M0_;
   HypreParMatrix * S0_;

   HypreParMatrix * DKZ_;
   HypreParMatrix * DKZT_;
   /*
    HypreParMatrix * G_;
    HypreParMatrix * GT_;
    HypreParMatrix * Z_;
    HypreParMatrix * ZT_;
   */
   // HypreParMatrix * NegG_;
   // HypreParMatrix * NegGT_;
   // HypreParMatrix * NegZ_;
   // HypreParMatrix * NegZT_;

   HypreSolver    * S0Inv_;

   BlockDiagonalPreconditioner * BDP_;
   HypreSolver * DP0_;
};

class LinearCombinationIntegrator : public BilinearFormIntegrator
{
private:
   int own_integrators;
   DenseMatrix elem_mat;
   Array<BilinearFormIntegrator*> integrators;
   Array<double> c;

public:
   LinearCombinationIntegrator(int own_integs = 1)
   { own_integrators = own_integs; }

   void AddIntegrator(double scalar, BilinearFormIntegrator *integ)
   { integrators.Append(integ); c.Append(scalar); }

   virtual void AssembleElementMatrix(const FiniteElement &el,
                                      ElementTransformation &Trans,
                                      DenseMatrix &elmat);

   virtual ~LinearCombinationIntegrator();
};

class ShiftedGradientIntegrator: public BilinearFormIntegrator
{
public:
   ShiftedGradientIntegrator(VectorCoefficient &vq,
                             const IntegrationRule *ir = NULL)
      : BilinearFormIntegrator(ir), VQ(&vq) { }

   virtual void AssembleElementMatrix(const FiniteElement &el,
                                      ElementTransformation &Trans,
                                      DenseMatrix &elmat);

private:
   Vector      shape;
   DenseMatrix dshape;
   DenseMatrix dshapedxt;
   DenseMatrix invdfdx;
   Vector      D;
   VectorCoefficient *VQ;
};

void * mfem_BlockOperatorMatvecCreate( void *A,
                                       void *x )
{
   void *matvec_data;

   matvec_data = NULL;

   return ( matvec_data );
}

HYPRE_Int mfem_BlockOperatorMatvec( void *matvec_data,
                                    HYPRE_Complex alpha,
                                    void *A,
                                    void *x,
                                    HYPRE_Complex beta,
                                    void *y )
{
   BlockOperator *Aop = (BlockOperator*)A;

   int width = Aop->Width();

   hypre_ParVector * xPar = (hypre_ParVector *)x;
   hypre_ParVector * yPar = (hypre_ParVector *)y;

   Vector xVec(xPar->local_vector->data, width);
   Vector yVec(yPar->local_vector->data, width);

   Aop->Mult( xVec, yVec );

   return 0;
}

HYPRE_Int mfem_BlockOperatorMatvecDestroy( void *matvec_data )
{
   return 0;
}

HYPRE_Int
mfem_BlockOperatorAMGSolve(void *solver,
                           void *A,
                           void *b,
                           void *x)
{
   BlockOperator *Aop = (BlockOperator*)A;

   int width = Aop->Width();

   hypre_ParVector * bPar = (hypre_ParVector *)b;
   hypre_ParVector * xPar = (hypre_ParVector *)x;

   Vector bVec(bPar->local_vector->data, width);
   Vector xVec(xPar->local_vector->data, width);

   Aop->Mult( bVec, xVec );

   return 0;
}

HYPRE_Int
mfem_BlockOperatorAMGSetup(void *solver,
                           void *A,
                           void *b,
                           void *x)
{
   return 0;
}

// Material Coefficients
double mass_coef(const Vector &);
double stiffness_coef(const Vector &);

int main(int argc, char *argv[])
{
   // 1. Initialize MPI.
   int num_procs, myid;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);

   // 2. Parse command-line options.
   const char *mesh_file = "../../data/periodic-cube.mesh";
   int order = 1;
   int sr = 0, pr = 2;
   bool visualization = 1;
   int nev = 5;
   double beta = 1.0;
   double alpha_a = 0.0, alpha_i = 0.0;
   double sigma = 0.0;

   OptionsParser args(argc, argv);
   args.AddOption(&mesh_file, "-m", "--mesh",
                  "Mesh file to use.");
   args.AddOption(&order, "-o", "--order",
                  "Finite element order (polynomial degree) or -1 for"
                  " isoparametric space.");
   args.AddOption(&sr, "-sr", "--serial-refinement",
                  "Number of serial refinement levels.");
   args.AddOption(&pr, "-pr", "--parallel-refinement",
                  "Number of parallel refinement levels.");
   args.AddOption(&nev, "-nev", "--num_eigs",
                  "Number of eigenvalues requested.");
   args.AddOption(&visualization, "-vis", "--visualization", "-no-vis",
                  "--no-visualization",
                  "Enable or disable GLVis visualization.");
   args.AddOption(&beta, "-b", "--phase-shift",
                  "Phase Shift Magnitude in degrees");
   args.AddOption(&alpha_a, "-az", "--azimuth",
                  "Azimuth in degrees");
   args.AddOption(&alpha_i, "-inc", "--inclination",
                  "Inclination in degrees");
   args.AddOption(&sigma, "-s", "--shift",
                  "Shift parameter for eigenvalue solve");
   args.Parse();
   if (!args.Good())
   {
      if (myid == 0)
      {
         args.PrintUsage(cout);
      }
      MPI_Finalize();
      return 1;
   }
   if (myid == 0)
   {
      args.PrintOptions(cout);
   }

   // 3. Read the (serial) mesh from the given mesh file on all processors.  We
   //    can handle triangular, quadrilateral, tetrahedral, hexahedral, surface
   //    and volume meshes with the same code.
   Mesh *mesh;
   ifstream imesh(mesh_file);
   if (!imesh)
   {
      if (myid == 0)
      {
         cerr << "\nCan not open mesh file: " << mesh_file << '\n' << endl;
      }
      MPI_Finalize();
      return 2;
   }
   mesh = new Mesh(imesh, 1, 1);
   imesh.close();

   // 4. Refine the serial mesh on all processors to increase the resolution. In
   //    this example we do 'ref_levels' of uniform refinement. We choose
   //    'ref_levels' to be the largest number that gives a final mesh with no
   //    more than 10,000 elements.
   {
      int ref_levels = sr;
      for (int l = 0; l < ref_levels; l++)
      {
         mesh->UniformRefinement();
      }
   }

   // 5. Define a parallel mesh by a partitioning of the serial mesh. Refine
   //    this mesh further in parallel to increase the resolution. Once the
   //    parallel mesh is defined, the serial mesh can be deleted.
   ParMesh *pmesh = new ParMesh(MPI_COMM_WORLD, *mesh);
   delete mesh;
   {
      int par_ref_levels = pr;
      for (int l = 0; l < par_ref_levels; l++)
      {
         pmesh->UniformRefinement();
      }
   }

   /*
   Vector ZetaVec(3);
   ZetaVec[0] = cos(alpha_i*M_PI/180.0)*cos(alpha_a*M_PI/180.0);
   ZetaVec[1] = cos(alpha_i*M_PI/180.0)*sin(alpha_a*M_PI/180.0);
   ZetaVec[2] = sin(alpha_i*M_PI/180.0);
   VectorConstantCoefficient zeta(ZetaVec); // this really should be k*zeta
   */

   L2_ParFESpace * L2FESpace = new L2_ParFESpace(pmesh,0,pmesh->Dimension());

   int nElems = L2FESpace->GetVSize();
   cout << myid << ": nElems = " << nElems << endl;

   ParGridFunction * m = new ParGridFunction(L2FESpace);
   ParGridFunction * k = new ParGridFunction(L2FESpace);

   FunctionCoefficient mFunc(mass_coef);
   FunctionCoefficient kFunc(stiffness_coef);

   m->ProjectCoefficient(mFunc);
   k->ProjectCoefficient(kFunc);

   // *m = 1.0;
   // *k = 1.0;
   if (visualization)
   {
      socketstream m_sock, k_sock;
      char vishost[] = "localhost";
      int  visport   = 19916;

      m_sock.open(vishost, visport);
      k_sock.open(vishost, visport);
      m_sock.precision(8);
      k_sock.precision(8);

      m_sock << "parallel " << num_procs << " " << myid << "\n"
             << "solution\n" << *pmesh << *m << flush
             << "window_title 'Mass Coefficient'\n" << flush;

      k_sock << "parallel " << num_procs << " " << myid << "\n"
             << "solution\n" << *pmesh << *k << flush
             << "window_title 'Stiffness Coefficient'\n" << flush;
   }

   GridFunctionCoefficient mCoef(m);
   GridFunctionCoefficient kCoef(k);

   ScalarFloquetWaveEquation eq(*pmesh, order);

   eq.SetBeta(beta);
   eq.SetAzimuth(alpha_a);
   eq.SetInclination(alpha_i);

   eq.SetMassCoef(mCoef);
   eq.SetStiffnessCoef(kCoef);

   eq.SetSigma(sigma);

   eq.Setup();

   cout << "Creating v" << endl;
   int * col_part = eq.GetTrueDofOffsets();
   HypreParVector v(MPI_COMM_WORLD,col_part[num_procs],col_part);

   cout << "   HYPRE_ParCSRSetupMatvec(&matvec_fn)" << endl;
   HYPRE_MatvecFunctions matvec_fn;
   // HYPRE_ParCSRSetupMatvec(&matvec_fn);
   matvec_fn.MatvecCreate       = mfem_BlockOperatorMatvecCreate;
   matvec_fn.Matvec             = mfem_BlockOperatorMatvec;
   matvec_fn.MatvecDestroy      = mfem_BlockOperatorMatvecDestroy;
   matvec_fn.MatMultiVecCreate  = NULL;
   matvec_fn.MatMultiVec        = NULL;
   matvec_fn.MatMultiVecDestroy = NULL;

   cout << "interpreter" << endl;
   mv_InterfaceInterpreter * interpreter =
      hypre_CTAlloc(mv_InterfaceInterpreter,1);
   HYPRE_ParCSRSetupInterpreter(interpreter);

   cout << "multi vector" << endl;
   mv_MultiVectorPtr eigenvectors =
      mv_MultiVectorCreateFromSampleVector(interpreter,nev,(HYPRE_ParVector)v);
   /*
   mv_MultiVectorPtr blockVectorY =
     mv_MultiVectorCreateFromSampleVector(interpreter,nev,(HYPRE_ParVector)v);
   */
   double * eigenvalues = (double*) calloc( nev, sizeof(double) );

   cout << "random" << endl;
   int lobpcgSeed = 775;    /* random seed */
   mv_MultiVectorSetRandom(eigenvectors, lobpcgSeed);

   // cout << "lobpcg create" << endl;
   int maxIterations = 300;
   int pcgMode = 1;
   int verbosity = 1;
   double tol = 1e-6;

   ParArPackSym arpack(MPI_COMM_WORLD);
   HypreGMRES   gmres(*(eq.GetOperator()));

   gmres.SetTol(1e-8);
   gmres.SetMaxIter(1000);
   gmres.SetPrintLevel(0);

   arpack.SetMaxIter(400);
   arpack.SetTol(1e-4);
   arpack.SetShift(sigma);
   arpack.SetMode(3);

   // arpack.SetA(*(eq.GetAOperator()));
   arpack.SetB(*(eq.GetMOperator()));
   arpack.SetSolver(gmres);

   /*
   HYPRE_Solver lobpcg_solver;
   HYPRE_LOBPCGCreate(interpreter, &matvec_fn, &lobpcg_solver);
   HYPRE_LOBPCGSetMaxIter(lobpcg_solver, maxIterations);
   HYPRE_LOBPCGSetPrecondUsageMode(lobpcg_solver, pcgMode);
   HYPRE_LOBPCGSetTol(lobpcg_solver, tol);
   HYPRE_LOBPCGSetPrintLevel(lobpcg_solver, verbosity);

   eq.SetSigma(sigma);

   HYPRE_LOBPCGSetPrecond(lobpcg_solver,
           (HYPRE_PtrToSolverFcn)mfem_BlockOperatorAMGSolve,
           (HYPRE_PtrToSolverFcn)mfem_BlockOperatorAMGSetup,
           (HYPRE_Solver)eq.GetBDP());

   HYPRE_LOBPCGSetupB(lobpcg_solver,(HYPRE_Matrix)eq.GetMOperator(),
            (HYPRE_Vector)((HYPRE_ParVector)v));

   // if ( fabs(beta) > 1e-2 || true )
   // {
   // HYPRE_LOBPCGSetupT(lobpcg_solver,(HYPRE_Matrix)eq.GetTOperator(),
   //       (HYPRE_Vector)((HYPRE_ParVector)v));
     // }

   HYPRE_LOBPCGSetup(lobpcg_solver,(HYPRE_Matrix)eq.GetAOperator(),
           (HYPRE_Vector)((HYPRE_ParVector)v),
           (HYPRE_Vector)((HYPRE_ParVector)v));

   HYPRE_LOBPCGSolve(lobpcg_solver, NULL, eigenvectors, eigenvalues );

   HYPRE_LOBPCGDestroy(lobpcg_solver);
   */
   HYPRE_ParVector* evecs;

   {
      mv_TempMultiVector* tmp =
         (mv_TempMultiVector*)mv_MultiVectorGetData(eigenvectors);
      evecs = (HYPRE_ParVector*)(tmp -> vector);
   }

   eq.PerturbEigenvalues(nev, tol, eigenvalues, evecs);

   if (visualization)
   {
      socketstream ur_sock, ui_sock;
      char vishost[] = "localhost";
      int  visport   = 19916;

      ur_sock.open(vishost, visport);
      ui_sock.open(vishost, visport);
      ur_sock.precision(8);
      ui_sock.precision(8);

      ParGridFunction ur(eq.GetFESpace());
      ParGridFunction ui(eq.GetFESpace());

      int h1_loc_size = eq.GetFESpace()->TrueVSize();

      HypreParVector urVec(eq.GetFESpace()->GetComm(),
                           eq.GetFESpace()->GlobalTrueVSize(),
                           NULL,
                           eq.GetFESpace()->GetTrueDofOffsets());
      HypreParVector uiVec(eq.GetFESpace()->GetComm(),
                           eq.GetFESpace()->GlobalTrueVSize(),
                           NULL,
                           eq.GetFESpace()->GetTrueDofOffsets());

      for (int e=0; e<nev; e++)
      {
         if ( myid == 0 )
         {
            cout << e << ":  Eigenvalue " << eigenvalues[e];
            double trans_eig = eigenvalues[e] - sigma;
            if ( fabs(sigma) > 1e-3 )
            {
               cout << ", transformed eigenvalue " << trans_eig;
            }
            if ( trans_eig > 0.0 )
            {
               cout << ", omega " << sqrt(trans_eig);
            }
            cout << endl;
         }

         double * data = hypre_VectorData(hypre_ParVectorLocalVector(
                                             (hypre_ParVector*)evecs[e]));
         urVec.SetData(&data[0]);
         uiVec.SetData(&data[h1_loc_size]);

         ur = urVec;
         ui = uiVec;

         ur_sock << "parallel " << num_procs << " " << myid << "\n"
                 << "solution\n" << *pmesh << ur << flush
                 << "window_title 'Re(u)'\n" << flush;
         ui_sock << "parallel " << num_procs << " " << myid << "\n"
                 << "solution\n" << *pmesh << ui << flush
                 << "window_title 'Im(u)'\n" << flush;

         char c;
         if (myid == 0)
         {
            cout << "press (q)uit or (c)ontinue --> " << flush;
            cin >> c;
         }
         MPI_Bcast(&c, 1, MPI_CHAR, 0, MPI_COMM_WORLD);

         if (c != 'c')
         {
            break;
         }
      }
      ur_sock.close();
      ui_sock.close();
   }

   hypre_TFree(eigenvalues);
   hypre_TFree(interpreter);
   /*
   lobpcg_BLASLAPACKFunctions blap_fn;
   blap_fn.dsygv = dsygv_interface;
   blap_fn.dpotrf = dpotrf_interface;
   */
   /*
   double tol = 1e-4;
   int maxIter = 100;
   int iter = 0;
   Array<double> lam(nev);
   Array<double> res(nev);
   lobpcg_solve(eigenvectors,NULL,,NULL,,NULL,NULL,blockVectorY,blap_fn,tol,maxIter,1,&iter,lam,NULL,0,res,NULL,0);
   */


   delete pmesh;

   MPI_Finalize();

   cout << "Exiting Main" << endl;

   return 0;
}

double mass_coef(const Vector & x)
{
   // if ( x.Norml2() <= 0.5 )
   if ( fabs(x(0)) <= 0.5 )
   {
      return 10.0;
   }
   else
   {
      return 1.0;
   }
}

double stiffness_coef(const Vector &x)
{
   // if ( x.Norml2() <= 0.5 )
   if ( fabs(x(0)) <= 0.5 )
   {
      return 5.0;
   }
   else
   {
      return 0.1;
   }
}

ScalarFloquetWaveEquation::ScalarFloquetWaveEquation(ParMesh & pmesh,
                                                     int order)
   : H1FESpace_(NULL),
     alpha_a_(0.0),
     alpha_i_(90.0),
     beta_(0.0),
     sigma_(0.0),
     mCoef_(NULL),
     kCoef_(NULL),
     A_(NULL),
     M_(NULL),
     A0_(NULL)
{
   int dim = pmesh.Dimension();

   zeta_.SetSize(dim);

   H1FESpace_    = new H1_ParFESpace(&pmesh,order,dim);

   block_offsets_.SetSize(3);
   block_offsets_[0] = 0;
   block_offsets_[1] = H1FESpace_->GetVSize();
   block_offsets_[2] = H1FESpace_->GetVSize();
   block_offsets_.PartialSum();

   block_trueOffsets_.SetSize(3);
   block_trueOffsets_[0] = 0;
   block_trueOffsets_[1] = H1FESpace_->TrueVSize();
   block_trueOffsets_[2] = H1FESpace_->TrueVSize();
   block_trueOffsets_.PartialSum();

   tdof_offsets_.SetSize(H1FESpace_->GetNRanks()+1);
   HYPRE_Int *    h1_tdof_offsets = H1FESpace_->GetTrueDofOffsets();
   for (int i=0; i<tdof_offsets_.Size(); i++)
   {
      tdof_offsets_[i] = 2 * h1_tdof_offsets[i];
   }

}

ScalarFloquetWaveEquation::~ScalarFloquetWaveEquation()
{
   if ( H1FESpace_    != NULL ) { delete H1FESpace_; }

   if ( A_  != NULL ) { delete A_; }
   if ( B_  != NULL ) { delete B_; }
   if ( M_  != NULL ) { delete M_; }
   if ( T_  != NULL ) { delete T_; }
   if ( Op_ != NULL ) { delete Op_; }

   if ( A0_ != NULL ) { delete A0_; }
   if ( M0_ != NULL ) { delete M0_; }
   if ( S0_ != NULL ) { delete S0_; }
   if ( S0Inv_ != NULL ) { delete S0Inv_; }

   if ( DKZ_  != NULL ) { delete DKZ_; }
   if ( DKZT_ != NULL ) { delete DKZT_; }
   /*
   if ( G_  != NULL ) delete G_;
   if ( GT_ != NULL ) delete GT_;
   if ( Z_  != NULL ) delete Z_;
   if ( ZT_ != NULL ) delete ZT_;
   */
   // if ( NegG_  != NULL ) delete NegG_;
   // if ( NegGT_ != NULL ) delete NegGT_;
   // if ( NegZ_  != NULL ) delete NegZ_;
   // if ( NegZT_ != NULL ) delete NegZT_;

   if ( BDP_   != NULL ) { delete BDP_; }
   if ( DP0_   != NULL ) { delete DP0_; }
}

void
ScalarFloquetWaveEquation::Setup()
{
   if ( zeta_.Size() == 3 )
   {
      zeta_[0] = cos(alpha_i_*M_PI/180.0)*cos(alpha_a_*M_PI/180.0);
      zeta_[1] = cos(alpha_i_*M_PI/180.0)*sin(alpha_a_*M_PI/180.0);
      zeta_[2] = sin(alpha_i_*M_PI/180.0);
   }
   else
   {
      zeta_[0] = cos(alpha_a_*M_PI/180.0);
      zeta_[1] = sin(alpha_a_*M_PI/180.0);
   }
   cout << "Zeta:  ";
   zeta_.Print(cout);

   VectorCoefficient * zCoef = NULL;

   if ( kCoef_ )
   {
      zCoef = new VectorFunctionCoefficient(zeta_,*kCoef_);
   }
   else
   {
      zCoef = new VectorConstantCoefficient(zeta_);
   }
   /*
   ParMixedBilinearForm z(HCurlFESpace_, H1FESpace_);
   z.AddDomainIntegrator(new VectorFEMassIntegrator(*zCoef));
   z.Assemble();
   z.Finalize();
   delete zCoef;

   Z_  = z.ParallelAssemble();// *Z_ *= beta_*M_PI/180.0;
   ZT_ = Z_->Transpose();
   */
   //NegZ_  = z.ParallelAssemble(); *NegZ_ *= -beta_*M_PI/180.0;
   //NegZT_ = NegZ_->Transpose();
   /*
   ParMixedBilinearForm g(H1FESpace_, HCurlFESpace_);
   g.AddDomainIntegrator(new VectorFEGradientIntegrator(*kCoef_));
   g.Assemble();
   g.Finalize();

   G_     = g.ParallelAssemble();
   GT_    = G_->Transpose();
   //NegG_  = g.ParallelAssemble(); *NegG_ *= -1.0;
   //NegGT_ = NegG_->Transpose();
   */
   /*
   ParDiscreteGradOperator g(H1FESpace_, HCurlFESpace_);
   HypreParMatrix * G  = g.ParallelAssemble();
   HypreParMatrix * GT = G_->Transpose();

   KZ_  = ParMult(GT,ZT_);
   KZT_ = ParMult(Z_,G);

   delete GT;
   */
   ParBilinearForm dkz(H1FESpace_);
   dkz.AddDomainIntegrator(new ShiftedGradientIntegrator(*zCoef));
   dkz.Assemble();
   dkz.Finalize();

   DKZ_  = dkz.ParallelAssemble();
   DKZT_ = DKZ_->Transpose();
   *DKZT_ *= -1.0;

   delete zCoef;

   cout << "Building S0" << endl;
   LinearCombinationIntegrator * bfi = new LinearCombinationIntegrator();
   if ( fabs(sigma_) > 0.0 )
   {
      bfi->AddIntegrator(-sigma_,
                         new MassIntegrator(*mCoef_));
   }
   if ( fabs(beta_) > 0.0 )
   {
      bfi->AddIntegrator(beta_*beta_*M_PI*M_PI/32400.0,
                         new MassIntegrator(*kCoef_));
   }
   bfi->AddIntegrator(1.0, new DiffusionIntegrator(*kCoef_));
   ParBilinearForm s0(H1FESpace_);
   s0.AddDomainIntegrator(bfi);
   s0.Assemble();
   s0.Finalize();
   S0_ = s0.ParallelAssemble();
   /*
   HypreParVector x0(*S0_);
   HypreParVector b0(*S0_,1);
   x0 = 1.0;
   S0_->Mult(x0,b0);
   cout << "norm of b0 " << b0.Normlinf() << endl;
   */
   S0Inv_ = new HypreBoomerAMG(*S0_);

   ParBilinearForm m0(H1FESpace_);
   m0.AddDomainIntegrator(new MassIntegrator(*mCoef_));
   m0.Assemble();
   m0.Finalize();
   M0_ = m0.ParallelAssemble();

   A_ = new BlockOperator(block_trueOffsets_);
   A_->SetDiagonalBlock(0,S0_);
   A_->SetDiagonalBlock(1,S0_);
   A_->owns_blocks = 0;

   B_ = new BlockOperator(block_trueOffsets_);
   B_->SetBlock(0,1,DKZ_,beta_*M_PI/180.0);
   B_->SetBlock(1,0,DKZT_,-beta_*M_PI/180.0);
   B_->owns_blocks = 0;

   M_ = new BlockOperator(block_trueOffsets_);
   M_->SetDiagonalBlock(0,M0_);
   M_->SetDiagonalBlock(1,M0_);
   M_->owns_blocks = 0;

   // if ( fabs(beta_) > 1e-2 || true )
   // {
   /*
     T_ = new BlockOperator(block_trueOffsets_);
     T_->SetDiagonalBlock(0,S0Inv_);
     T_->SetDiagonalBlock(1,S0Inv_);
     T_->owns_blocks = 0;
   */
   // }
   // else
   // {
   // T_ = NULL;
   // }

   M0_->Print("M0.mat");
   S0_->Print("S0.mat");
   // G_->Print("G.mat");
   // Z_->Print("Z.mat");

   Op_ = new BlockOperator(block_trueOffsets_);
   Op_->SetDiagonalBlock(0,M0_,-1.0*sigma_);
   Op_->SetDiagonalBlock(1,M0_,-1.0*sigma_);
   Op_->owns_blocks = 0;
   /*
   DP0_ = new HypreDiagScale(*M0_); DP0_->iterative_mode = false;

   BDP_ = new BlockDiagonalPreconditioner(block_trueOffsets_);
   BDP_->SetDiagonalBlock(0,DP0_);
   BDP_->SetDiagonalBlock(1,DP0_);
   BDP_->owns_blocks = 0;
   */
   BDP_ = new BlockDiagonalPreconditioner(block_trueOffsets_);
   BDP_->SetDiagonalBlock(0,S0Inv_);
   BDP_->SetDiagonalBlock(1,S0Inv_);
   BDP_->owns_blocks = 0;

   {
      cout << "Building A0" << endl;
      LinearCombinationIntegrator * bfi = new LinearCombinationIntegrator();
      // bfi->AddIntegrator(omega_*omega_, new MassIntegrator(*mCoef_));
      bfi->AddIntegrator(-1.0, new DiffusionIntegrator(*kCoef_));
      ParBilinearForm a0(H1FESpace_);
      a0.AddDomainIntegrator(bfi);
      a0.Assemble();
      a0.Finalize();
      A0_ = a0.ParallelAssemble();
   }
   cout << "Leaving Setup" << endl;
}

BlockOperator *
ScalarFloquetWaveEquation::GetOperator()
{
   // Reset the diagonal blocks in case sigma has changed
   Op_->SetDiagonalBlock(0,M0_,-1.0*sigma_);
   Op_->SetDiagonalBlock(1,M0_,-1.0*sigma_);

   return Op_;
}

void
ScalarFloquetWaveEquation::PerturbEigenvalues(int nev,
                                              double tol,
                                              double * lam0,
                                              HYPRE_ParVector* v0)
{
   map<int,set<int> > dgen_lam0;

   for (int i=0; i<nev; i++)
   {
      cout << (int)rint(lam0[i]/tol) << "\t" << lam0[i] << endl;
      dgen_lam0[(int)rint(lam0[i]/tol)].insert(i);
   }

   // HypreParVector tmp(H1FESpace_);
   HypreParVector Bvi(H1FESpace_->GetComm(),
                      2*H1FESpace_->GlobalVSize(),
                      tdof_offsets_);
   HypreParVector Mvi(H1FESpace_->GetComm(),
                      2*H1FESpace_->GlobalVSize(),
                      tdof_offsets_);

   for (map<int,set<int> >::iterator mit=dgen_lam0.begin();
        mit != dgen_lam0.end(); mit++)
   {
      cout << "Degenerate Space of Size:  " << mit->second.size() << endl;
      DenseMatrix vBv(mit->second.size());
      DenseMatrix vMv(mit->second.size());
      int i = 0;
      for (set<int>::iterator si=mit->second.begin();
           si!=mit->second.end(); si++)
      {
         HypreParVector vi(v0[*si]);
         B_->Mult(vi,Bvi);
         M_->Mult(vi,Mvi);
         int j = 0;
         for (set<int>::iterator sj=mit->second.begin();
              sj!=mit->second.end(); sj++)
         {
            HypreParVector vj(v0[*sj]);
            vBv(j,i) = InnerProduct(vj,Bvi);
            vMv(j,i) = InnerProduct(vj,Mvi);
            cout << j << " " << i << "\t" << vBv(j,i) << "\t" << vMv(j,i) << endl;
            j++;
         }
         i++;
      }
      Vector dlam_real, dlam_imag;
      DenseMatrix dv;
      vBv.Eigensystem(dlam_real, dlam_imag, dv);
      // dgeev_Eigensystem(vBv,dlam,&dv);
      // dlam.Print(cout);
   }
}

void LinearCombinationIntegrator::AssembleElementMatrix(
   const FiniteElement &el, ElementTransformation &Trans, DenseMatrix &elmat)
{
   MFEM_ASSERT(integrators.Size() > 0, "empty LinearCombinationIntegrator.");

   integrators[0]->AssembleElementMatrix(el, Trans, elmat);
   elmat *= c[0];
   for (int i = 1; i < integrators.Size(); i++)
   {
      integrators[i]->AssembleElementMatrix(el, Trans, elem_mat);
      elem_mat *= c[i];
      elmat += elem_mat;
   }
}

LinearCombinationIntegrator::~LinearCombinationIntegrator()
{
   if (own_integrators)
   {
      for (int i = 0; i < integrators.Size(); i++)
      {
         delete integrators[i];
      }
   }
}

void ShiftedGradientIntegrator::AssembleElementMatrix
( const FiniteElement &el, ElementTransformation &Trans,
  DenseMatrix &elmat )
{
   int nd = el.GetDof();
   int dim = el.GetDim();
   double w;

   elmat.SetSize(nd);
   shape.SetSize(nd);
   dshape.SetSize(nd,dim);
   dshapedxt.SetSize(nd,dim);
   invdfdx.SetSize(dim,dim);
   D.SetSize(dim);

   elmat = 0.0;

   const IntegrationRule *ir = IntRule;
   if (ir == NULL)
   {
      // int order = 2 * el.GetOrder();
      int order = 2 * el.GetOrder() + Trans.OrderW();
      ir = &IntRules.Get(el.GetGeomType(), order);
   }

   for (int i = 0; i < ir->GetNPoints(); i++)
   {
      const IntegrationPoint &ip = ir->IntPoint(i);
      el.CalcShape(ip, shape);
      el.CalcDShape(ip, dshape);

      Trans.SetIntPoint (&ip);
      CalcAdjugate(Trans.Jacobian(), invdfdx);
      w = Trans.Weight() * ip.weight;
      Mult(dshape, invdfdx, dshapedxt);

      VQ -> Eval(D, Trans, ip);
      D *= w;

      for (int d = 0; d < dim; d++)
      {
         for (int j = 0; j < nd; j++)
         {
            for (int k = 0; k < nd; k++)
            {
               elmat(j, k) += dshapedxt(j,d) * D[d] * shape(k)
                              - shape(j) * D[d] * dshapedxt(k,d);
            }
         }
      }
   }
}
