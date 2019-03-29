// Copyright (c) 2010, Lawrence Livermore National Security, LLC. Produced at
// the Lawrence Livermore National Laboratory. LLNL-CODE-443211. All Rights
// reserved. See file COPYRIGHT for details.
//
// This file is part of the MFEM library. For more information and source code
// availability see http://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License (as published by the Free
// Software Foundation) version 2.1 dated February 1999.

#ifndef MFEM_BRAVAIS
#define MFEM_BRAVAIS

#include "../../config/config.hpp"

#include "mfem.hpp"
#include "../common/pfem_extras.hpp"

namespace mfem
{

namespace bravais
{

enum BRAVAIS_LATTICE_TYPE
{
   INVALID_TYPE = 0,
   // 2D Bravais Lattices (5 types)
   PRIMITIVE_SQUARE,
   PRIMITIVE_HEXAGONAL,
   PRIMITIVE_RECTANGULAR,
   CENTERED_RECTANGULAR,
   PRIMITIVE_OBLIQUE,
   // 3D Bravais Lattices (14 types)
   PRIMITIVE_CUBIC,
   FACE_CENTERED_CUBIC,
   BODY_CENTERED_CUBIC,
   PRIMITIVE_TETRAGONAL,
   BODY_CENTERED_TETRAGONAL,
   PRIMITIVE_ORTHORHOMBIC,
   FACE_CENTERED_ORTHORHOMBIC,
   BODY_CENTERED_ORTHORHOMBIC,
   BASE_CENTERED_ORTHORHOMBIC,
   PRIMITIVE_HEXAGONAL_PRISM,
   PRIMITIVE_RHOMBOHEDRAL,
   PRIMITIVE_MONOCLINIC,
   BASE_CENTERED_MONOCLINIC,
   PRIMITIVE_TRICLINIC
};

/**
   The Symmetry points and their labels are taken from the various
   primitive unit cells described in the preprint "High-throughput
   electronic band structure calculations: challenges and tools" by
   Wahyu Setyawan and Stefano Curtarolo available on arxiv.org as
   arXiv:1004.2974v1.
*/

/** The Lattice class defines a simple interface for a generic
    topological lattice.
 */
// class Lattice
// {
// public:
//    inline double GetDim() const { return dim_; }
//    inline double GetUnitCellVolume() const { return vol_; }

//    void GetLatticeVectors(std::vector<Vector> & a) const;
//    void GetReciprocalLatticeVectors(std::vector<Vector> & b) const;

//    /** The Primitive Cell is the smallest volume that can tile all of
//        space without voids and contains only one lattice point at the
//        origin of the cordinate system.  The Primitive Cell is a
//        Fundamental Domain with regard to Translational symmetries
//        only. */
//    bool MapToPrimitiveCell(const Vector & pt, Vector & ipt) const;

//    /** The Fundamental Domain is a connected subset of the Primitive
//        Cell which can generate the entire Primitive Cell under the
//        action of a set of reflection symmetries. */
//    virtual bool MapToFundamentalDomain(const Vector & pt,
//                                        Vector & ipt) const = 0;

// protected:
//    /// Constructor requiring the dimension of the lattice space
//    Lattice(unsigned int dim);

//    /// Initializes the volume of the primitive cell
//    void SetUnitCellVolume();

//    unsigned int dim_;
//    double       vol_;

//    std::vector< Vector > lat_vecs_; // Lattice Vectors
//    std::vector< Vector > rec_vecs_; // Reciprocal Lattice Vectors
// };
/*
class BravaisLattice : public virtual Lattice
{
public:
   virtual ~BravaisLattice() {}

   // Maps an arbitrary point pt to the point ipt in the Wigner-Seitz
   // Cell (equivalent to the Primitive Cell in the Lattice class).
   // Returns true if pt required mapping.  In other words the return
   // value is (pt != ipt)
   virtual bool MapToWignerSeitzCell(const Vector & pt, Vector & ipt) const;

   // Maps the point pt in the Wigner-Seitz Cell to the point ipt in the
   // Fundamental Domain.  Returns true if pt required
   // mapping.  In other words the return value is (pt != ipt)
   virtual bool MapToReducedWignerSeitzCell(const Vector & pt,
                   Vector & ipt) const;

protected:
  BravaisLattice(unsigned int dim): Lattice(dim) {}
};
*/
// class ReciprocalLattice : public virtual Lattice
// {
// public:

//    // Maps an arbitrary point pt to the point ipt in the
//    // First Brillouin Zone.  Returns true if pt required
//    // mapping.  In other words the return value is (pt != ipt)
//    virtual bool MapToBrilouinZone(const Vector & pt, Vector & ipt) const;

//    // Maps the point pt in the First Brillouin Zone to the point ipt in the
//    // Irreducible Brillouin Zone.  Returns true if pt required
//    // mapping.  In other words the return value is (pt != ipt)
//    virtual bool MapToIrreducibleBrillouinZone(const Vector & pt,
//                                               Vector & ipt) const;

//    // The Symmetry points are the vertices of the Irreducible Brillouin Zone.
//    virtual unsigned int GetNumberSymmetryPoints() = 0;

//    // The Intermediate Points are the midpoints of the edges of the
//    // Irreducible Brillouin Zone.
//    virtual unsigned int GetNumberIntermediatePoints() = 0;

//    // The Paths are sequences of edges that traverse every edge of the
//    // Irreducible Brillouin Zone exactly once.
//    virtual unsigned int GetNumberPaths() = 0;
//    virtual unsigned int GetNumberPathSegments(int i) = 0;

//    void          GetSymmetryPoint(int i, Vector & pt);
//    std::string & GetSymmetryPointLabel(int i);
//    int           GetSymmetryPointIndex(const std::string & label);

//    void          GetIntermediatePoint(int p, int s, Vector & pt);
//    std::string & GetIntermediatePointLabel(int p, int s);

//    void GetPathSegmentEndPointIndices(int p, int s, int & e0, int & e1);

// protected:
//    ReciprocalLattice(unsigned int dim) : Lattice(dim) {}

//    void SetVectorSizes();
//    void SetIntermediatePoints();

//    std::vector< Vector > sp_;
//    std::vector< std::string > sl_;
//    std::map< std::string, int >  si_;

//    std::vector< std::vector< Vector > > ip_;
//    std::vector< std::vector< std::string > > il_;
//    std::vector< std::vector< int    > > path_;
// };
/*
class CubicLattice : public virtual Lattice
{
public:
 CubicLattice(double a = 1.0);

 virtual bool MapToFundamentalDomain(const Vector & pt,
              Vector & ipt) const;

protected:
 double a_; // Lattice spacing
};

class FaceCenteredCubicLattice : public virtual Lattice
{
public:
 FaceCenteredCubicLattice(double a = 1.0);

 virtual bool MapToFundamentalDomain(const Vector & pt,
              Vector & ipt) const;

private:
 double a_;
};

class BodyCenteredCubicLattice : public virtual Lattice
{
public:
 BodyCenteredCubicLattice(double a = 1.0);

 virtual bool MapToFundamentalDomain(const Vector & pt,
              Vector & ipt) const;

private:
 double a_;
};

class HexagonalLattice : public virtual Lattice
{
public:
 HexagonalLattice(double a = 1.0, double c = 1.0);

 virtual bool MapToFundamentalDomain(const Vector & pt,
              Vector & ipt) const;

private:
 double a_;
 double c_;
};
*/
/*
class CubicBravaisLattice : public BravaisLattice, public CubicLattice
{
public:
 CubicBravaisLattice(double a = 1.0)
   : Lattice(3), BravaisLattice(3), CubicLattice(a) {}
};
*/
/*
class CubicBrillouinZone : public ReciprocalLattice, public CubicLattice
{
public:
 CubicBrillouinZone(double a = 1.0);

 virtual unsigned int GetNumberSymmetryPoints()     { return 4; }
 virtual unsigned int GetNumberIntermediatePoints() { return 6; }
 virtual unsigned int GetNumberPaths()              { return 2; }
 virtual unsigned int GetNumberPathSegments(int i)  { return (i==0)?5:1; }
};

class FaceCenteredCubicBrillouinZone : public ReciprocalLattice,
              public FaceCenteredCubicLattice
{
public:
 FaceCenteredCubicBrillouinZone(double a = 1.0);

 virtual unsigned int GetNumberSymmetryPoints()     { return 6; }
 virtual unsigned int GetNumberIntermediatePoints() { return 10; }
 virtual unsigned int GetNumberPaths()              { return 2; }
 virtual unsigned int GetNumberPathSegments(int i)  { return (i==0)?9:1; }
};

class BodyCenteredCubicBrillouinZone : public ReciprocalLattice,
              public BodyCenteredCubicLattice
{
public:
 BodyCenteredCubicBrillouinZone(double a = 1.0);

 virtual unsigned int GetNumberSymmetryPoints()     { return 4; }
 virtual unsigned int GetNumberIntermediatePoints() { return 6; }
 virtual unsigned int GetNumberPaths()              { return 2; }
 virtual unsigned int GetNumberPathSegments(int i)  { return (i==0)?5:1; }
};

class HexagonalBrillouinZone : public ReciprocalLattice,
           public HexagonalLattice
{
public:
 HexagonalBrillouinZone(double a = 1.0, double c = 1.0);

 virtual unsigned int GetNumberSymmetryPoints()     { return 6; }
 virtual unsigned int GetNumberIntermediatePoints() { return 9; }
 virtual unsigned int GetNumberPaths()              { return 3; }
 virtual unsigned int GetNumberPathSegments(int i)  { return (i==0)?7:1; }
};
*/
/**  Bravais Lattices are parameterized by three lengths (a, b, and c)
     and three angles (alpha, beta, gamma).

           *-------------*
          / \           / \
         /   \ _gamma  /   \
        /     \  \    /     \
       /       \  |  /       \
      /         *-------------*
     /         /   /         /
    /         /   /         /
   *---------/---*   beta  /
    \       /     \   _   / c
     \     /       \ / \ /
    a \   /_ alpha  \   /
       \ /   \       \ /
        *-------------*
               b

     They are also distinguished by a centering type.  The centerings
     are: Primitive, Face-Centered, Body-Centered, and Base-Centered.

| Name            |  a  |  b  |   c  |      alpha     |   beta  |  gamma  |
|-----------------|-----|-----|------|----------------|---------|---------|
| Square          | > 0 | = a |  N/A |       N/A      |   N/A   |  = pi/2 |
| Hexagonal       | > 0 | = a |  N/A |       N/A      |   N/A   |  = pi/3 |
| Cubic           | > 0 | = a |  = a |     = pi/2     |  = pi/2 |  = pi/2 |
| Tetragonal      | > 0 | = a | != a |     = pi/2     |  = pi/2 |  = pi/2 |
| Orthorhombic    | > 0 | > a |  > b |     = pi/2     |  = pi/2 |  = pi/2 |
| Hexagonal Prism | > 0 | = a |  > 0 |     = pi/2     |  = pi/2 |  = pi/3 |
| Rhombohedral    | > 0 | = a |  = a | > 0 && < 2pi/3 | = alpha | = alpha |
| Monoclinic      | > 0 | > 0 | >= b | > arcsec(2c/b) |  = pi/2 |  = pi/2 |
|                 |     |     |      |   && < pi/2    |         |         |
| Triclinic       | > 0 | > 0 |  > 0 |    != pi/2     | != pi/2 | != pi/2 |

 */
class BravaisLattice
{
public:
   virtual ~BravaisLattice() {}

   inline BRAVAIS_LATTICE_TYPE GetLatticeType() { return type_; }
   inline std::string &   GetLatticeTypeLabel() { return label_; }

   inline unsigned int GetDim() const { return dim_; }
   inline double GetUnitCellVolume() const { return vol_; }
   inline double GetBrillouinZoneVolume() const { return bz_vol_; }

   void GetLatticeVectors(std::vector<Vector> & a) const;
   void GetReciprocalLatticeVectors(std::vector<Vector> & b) const;

   // These vectors give the location of the nearest neighbors of the
   // fundamental domain.  They also can be used to map nodes between
   // opposite faces of the unit cell.
   void GetTranslationVectors(std::vector<Vector> & t) const;

   // These are the radii of circles which can be inscribed within each
   // face.  This information can be used along with the translation
   // vectors to define cylindrical rods connecting the lattice points.
   void GetFaceRadii(std::vector<double> & r) const;

   // The Primitive Cell is the smallest volume that can tile all of
   // space without voids and contains only one lattice point at the
   // origin of the cordinate system.  The Primitive Cell is a
   // Fundamental Domain with regard to Translational symmetries only.
   // Returns true if the point required mapping
   // i.e. returns (ipt != pt).
   bool MapToPrimitiveCell(const Vector & pt, Vector & ipt) const;

   // The Fundamental Domain is a connected subset of the Primitive
   // Cell which can generate the entire Primitive Cell under the
   // action of a set of reflection symmetries.  Returns true if the
   // point required mapping i.e. returns (ipt != pt).
   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const = 0;

   // In this context "Symmetry Points" are points in the
   // reciprocal space.  They are sometimes called "High-Symmetry
   // Points" or "Critical Points" of the First Brillouin Zone.
   virtual unsigned int GetNumberSymmetryPoints() = 0;
   virtual unsigned int GetNumberIntermediatePoints() = 0;

   virtual unsigned int GetNumberPaths() = 0;
   virtual unsigned int GetNumberPathSegments(int i) = 0;

   void          GetSymmetryPoint(int i, Vector & pt);
   std::string & GetSymmetryPointLabel(int i);
   int           GetSymmetryPointIndex(const std::string & label);

   void          GetIntermediatePoint(int p, int s, Vector & pt);
   std::string & GetIntermediatePointLabel(int p, int s);

   void GetPathSegmentEndPointIndices(int p, int s, int & e0, int & e1);

   virtual mfem::Mesh * GetCoarseWignerSeitzMesh() const = 0;

protected:
   BravaisLattice(unsigned int dim);

   void SetVectorSizes();
   void SetIntermediatePoints();
   void SetCellVolumes();
   double ComputeCellVolume(const std::vector<Vector> & vecs);

   std::vector< Vector > lat_vecs_; // Lattice Vectors
   std::vector< Vector > rec_vecs_; // Recoprocal Lattice Vectors
   std::vector< Vector > trn_vecs_; // Translation Vectors
   std::vector<double> face_radii_; // Face Radii
   std::vector< Vector > sp_;
   std::vector< std::string > sl_;
   std::map< std::string, int >  si_;

   std::vector< std::vector< Vector > > ip_;
   std::vector< std::vector< std::string > > il_;
   std::vector< std::vector< int    > > path_;

   std::string label_;
   BRAVAIS_LATTICE_TYPE type_;

   unsigned int dim_;
   double vol_;
   double bz_vol_;
};

class BravaisLattice2D : public BravaisLattice
{
public:
   BravaisLattice2D(double a, double b, double gamma);

   void GetAxialLengths(double &a, double &b);
   void GetInteraxialAngle(double &gamma);

protected:
   double a_;
   double b_;
   double gamma_;
};

class BravaisLattice3D : public BravaisLattice
{
public:
   BravaisLattice3D(double a, double b, double c,
                    double alpha, double beta, double gamma);

   void GetAxialLengths(double &a, double &b, double &c);
   void GetInteraxialAngles(double &alpha, double &beta, double &gamma);

protected:
   double a_;
   double b_;
   double c_;
   double alpha_;
   double beta_;
   double gamma_;
};

class SquareLattice : public BravaisLattice2D
{
public:
   SquareLattice(double a = 1.0);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()     { return 3; }
   virtual unsigned int GetNumberIntermediatePoints() { return 3; }
   virtual unsigned int GetNumberPaths()              { return 1; }
   virtual unsigned int GetNumberPathSegments(int i)  { return 3; }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   // double a_;

   // Data for mesh of the corresponding Wigner-Setiz Cell
   double ws_vert_[12];  // Vertex coordinates
   int ws_e2v_[4];       // Element to vertex connectivity
   int ws_elem_att_[1];  // Element Attributes
   int ws_be2v_[8];      // Boundary Element to vertex connectivity
   int ws_belem_att_[4]; // Boundary element Attributes
};

class HexagonalLattice : public BravaisLattice2D
{
public:
   HexagonalLattice(double a = 1.0);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()     { return 3; }
   virtual unsigned int GetNumberIntermediatePoints() { return 3; }
   virtual unsigned int GetNumberPaths()              { return 1; }
   virtual unsigned int GetNumberPathSegments(int i)  { return 3; }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   // double a_;

   // Data for mesh of the corresponding Wigner-Setiz Cell
   double ws_vert_[21];  // Vertex coordinates
   int ws_e2v_[12];      // Element to vertex connectivity
   int ws_elem_att_[3];  // Element Attributes
   int ws_be2v_[12];     // Boundary Element to vertex connectivity
   int ws_belem_att_[6]; // Boundary element Attributes
};

class CubicLattice : public BravaisLattice3D
{
public:
   CubicLattice(double a = 1.0);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()     { return 4; }
   virtual unsigned int GetNumberIntermediatePoints() { return 6; }
   virtual unsigned int GetNumberPaths()              { return 2; }
   virtual unsigned int GetNumberPathSegments(int i)  { return (i==0)?5:1; }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   // double a_;

   // Data for mesh of the corresponding Wigner-Setiz Cell
   double ws_vert_[24];  // Vertex coordinates
   int ws_e2v_[8];       // Element to vertex connectivity
   int ws_elem_att_[1];  // Element Attributes
   int ws_be2v_[24];     // Boundary Element to vertex connectivity
   int ws_belem_att_[6]; // Boundary element Attributes

   double ws_vert_tet_[81];   // Vertex coordinates
   int ws_e2v_tet_[192];      // Element to vertex connectivity
   int ws_elem_att_tet_[48];  // Element Attributes
   int ws_be2v_tet_[144];     // Boundary Element to vertex connectivity
   int ws_belem_att_tet_[48]; // Boundary element Attributes
};

class FaceCenteredCubicLattice : public BravaisLattice3D
{
public:
   FaceCenteredCubicLattice(double a = 1.0);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()     { return 6; }
   virtual unsigned int GetNumberIntermediatePoints() { return 10; }
   virtual unsigned int GetNumberPaths()              { return 2; }
   virtual unsigned int GetNumberPathSegments(int i)  { return (i==0)?9:1; }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   //double a_;

   // Data for mesh of the corresponding Wigner-Setiz Cell
   double ws_vert_[45];   // Vertex coordinates
   int ws_e2v_[32];       // Element to vertex connectivity
   int ws_elem_att_[4];   // Element Attributes
   int ws_be2v_[48];      // Boundary Element to vertex connectivity
   int ws_belem_att_[12]; // Boundary element Attributes
};

class BodyCenteredCubicLattice : public BravaisLattice3D
{
public:
   BodyCenteredCubicLattice(double a = 1.0);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()     { return 4; }
   virtual unsigned int GetNumberIntermediatePoints() { return 6; }
   virtual unsigned int GetNumberPaths()              { return 2; }
   virtual unsigned int GetNumberPathSegments(int i)  { return (i==0)?5:1; }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   //double a_;

   // Data for mesh of the corresponding Wigner-Setiz Cell
   double ws_vert_[114];  // Vertex coordinates
   int ws_e2v_[128];      // Element to vertex connectivity
   int ws_elem_att_[16];  // Element Attributes
   int ws_be2v_[120];     // Boundary Element to vertex connectivity
   int ws_belem_att_[30]; // Boundary element Attributes
};

class TetragonalLattice : public BravaisLattice3D
{
public:
   TetragonalLattice(double a = 1.0, double c = 0.5);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()     { return 6; }
   virtual unsigned int GetNumberIntermediatePoints() { return 9; }
   virtual unsigned int GetNumberPaths()              { return 3; }
   virtual unsigned int GetNumberPathSegments(int i)  { return (i==0)?7:1; }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   // double a_;
   // double c_;

   // Data for mesh of the corresponding Wigner-Setiz Cell
   double ws_vert_[24];  // Vertex coordinates
   int ws_e2v_[8];       // Element to vertex connectivity
   int ws_elem_att_[1];  // Element Attributes
   int ws_be2v_[24];     // Boundary Element to vertex connectivity
   int ws_belem_att_[6]; // Boundary element Attributes
};

class BodyCenteredTetragonalLattice : public BravaisLattice3D
{
public:
   BodyCenteredTetragonalLattice(double a = 1.0, double c = 0.5);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   // There are two possible shapes for the Brillouin zone:
   // elongated dodecahedron when c < a, trucated octahedron
   // when c > a.
   virtual unsigned int GetNumberSymmetryPoints()     { return (c_<a_)?7:9; }
   virtual unsigned int GetNumberIntermediatePoints() { return (c_<a_)?9:11; }
   virtual unsigned int GetNumberPaths()              { return 2; }
   virtual unsigned int GetNumberPathSegments(int i)
   { return (c_<a_)?((i==0)?8:1):((i==0)?10:1); }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   double a_;
   double c_;

   // There are two possible shapes for the Wigner-Seitz cell:
   // elongated dodecahedron when c > sqrt(2) a, trucated octahedron
   // when c < sqrt(2) a. The following arrays will be sized to
   // accomodate the larger case.
   //
   // | type      |num nodes | num elems | num bdr elems |
   // |-----------|----------|-----------|---------------|
   // | elong-dod |    24    |     8     |      20       |
   // | trunc-oct |    38    |    16     |      30       |
   //
   void createElongatedDodecahedron();
   void createTruncatedOctahedron();

   double ws_vert_[114];  // Vertex coordinates
   int ws_e2v_[128];      // Element to vertex connectivity
   int ws_elem_att_[16];  // Element Attributes
   int ws_be2v_[120];     // Boundary Element to vertex connectivity
   int ws_belem_att_[30]; // Boundary element Attributes
};

class OrthorhombicLattice : public BravaisLattice3D
{
public:
   OrthorhombicLattice(double a = 0.5, double b = 0.8, double c = 1.0);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()     { return 8; }
   virtual unsigned int GetNumberIntermediatePoints() { return 12; }
   virtual unsigned int GetNumberPaths()              { return 4; }
   virtual unsigned int GetNumberPathSegments(int i)  { return (i==0)?9:1; }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   // double a_;
   // double b_;
   // double c_;

   // Data for mesh of the corresponding Wigner-Setiz Cell
   double ws_vert_[24];  // Vertex coordinates
   int ws_e2v_[8];       // Element to vertex connectivity
   int ws_elem_att_[1];  // Element Attributes
   int ws_be2v_[24];     // Boundary Element to vertex connectivity
   int ws_belem_att_[6]; // Boundary element Attributes
};

class FaceCenteredOrthorhombicLattice : public BravaisLattice3D
{
public:
   FaceCenteredOrthorhombicLattice(double a = 1.0, double b = 1.2,
                                   double c = 1.6);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()
   { return (variety_==1)?9:((variety_==2)?11:8); }
   virtual unsigned int GetNumberIntermediatePoints()
   { return (variety_==1)?11:((variety_==2)?13:10); }
   virtual unsigned int GetNumberPaths()
   { return (variety_==1)?4:((variety_==2)?5:3); }
   virtual unsigned int GetNumberPathSegments(int i)
   {
      switch (variety_)
      {
         case 1:
            return (i==0)?7:((i==2)?2:1);
            break;
         case 2:
            return (i==0)?9:1;
            break;
         case 3:
            return (i==0)?7:((i==1)?2:1);
            break;
         default:
            return 0;
      };
   }
   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   // double a_;
   // double b_;
   // double c_;
   int variety_;

   // Data for mesh of the corresponding Wigner-Setiz Cell
   double ws_vert_[114];  // Vertex coordinates
   int ws_e2v_[128];      // Element to vertex connectivity
   int ws_elem_att_[16];  // Element Attributes
   int ws_be2v_[120];     // Boundary Element to vertex connectivity
   int ws_belem_att_[30]; // Boundary element Attributes
};

class BodyCenteredOrthorhombicLattice : public BravaisLattice3D
{
public:
   BodyCenteredOrthorhombicLattice(double a = 1.0, double b = 1.2,
                                   double c = 1.6);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()     { return 13; }
   virtual unsigned int GetNumberIntermediatePoints() { return 13; }
   virtual unsigned int GetNumberPaths()              { return 3; }
   virtual unsigned int GetNumberPathSegments(int i)  { return (i==0)?11:1; }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   // double a_;
   // double b_;
   // double c_;

   // There are two possible shapes for the Wigner-Seitz cell:
   // elongated dodecahedron when c > sqrt(a^2+b^2), trucated octahedron
   // when c < sqrt(a^2+b^2). The following arrays will be sized to
   // accomodate the larger case.
   //
   // | type      |num nodes | num elems | num bdr elems |
   // |-----------|----------|-----------|---------------|
   // | elong-dod |    24    |     8     |      20       |
   // | trunc-oct |    38    |    16     |      30       |
   //
   void createElongatedDodecahedron();
   void createTruncatedOctahedron();

   double ws_vert_[114];  // Vertex coordinates
   int ws_e2v_[128];      // Element to vertex connectivity
   int ws_elem_att_[16];  // Element Attributes
   int ws_be2v_[120];     // Boundary Element to vertex connectivity
   int ws_belem_att_[30]; // Boundary element Attributes
};

class BaseCenteredOrthorhombicLattice : public BravaisLattice3D
{
public:
   BaseCenteredOrthorhombicLattice(double a = 0.5, double b = 1.0,
                                   double c = 1.0);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()     { return 10; }
   virtual unsigned int GetNumberIntermediatePoints() { return 12; }
   virtual unsigned int GetNumberPaths()              { return 2; }
   virtual unsigned int GetNumberPathSegments(int i)  { return (i==0)?11:1; }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   // double a_;
   // double b_;
   // double c_;

   // Data for mesh of the corresponding Wigner-Setiz Cell
   double ws_vert_[63];   // Vertex coordinates
   int ws_e2v_[48];       // Element to vertex connectivity
   int ws_elem_att_[6];   // Element Attributes
   int ws_be2v_[72];      // Boundary Element to vertex connectivity
   int ws_belem_att_[18]; // Boundary element Attributes
};

class HexagonalPrismLattice : public BravaisLattice3D
{
public:
   HexagonalPrismLattice(double a = 1.0, double c = 1.0);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()     { return 6; }
   virtual unsigned int GetNumberIntermediatePoints() { return 9; }
   virtual unsigned int GetNumberPaths()              { return 3; }
   virtual unsigned int GetNumberPathSegments(int i)  { return (i==0)?7:1; }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   // double a_;
   // double c_;

   // Data for mesh of the corresponding Wigner-Setiz Cell
   double ws_vert_[63];   // Vertex coordinates
   int ws_e2v_[48];       // Element to vertex connectivity
   int ws_elem_att_[6];   // Element Attributes
   int ws_be2v_[72];      // Boundary Element to vertex connectivity
   int ws_belem_att_[18]; // Boundary element Attributes
};

class RhombohedralLattice : public BravaisLattice3D
{
public:
   RhombohedralLattice(double a = 1.0, double alpha = 0.25 * M_PI);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()     { return (alpha_ < 0.5 * M_PI)?12:8; }
   virtual unsigned int GetNumberIntermediatePoints() { return 9; }
   virtual unsigned int GetNumberPaths()              { return (alpha_ < 0.5 * M_PI)?4:1; }
   virtual unsigned int GetNumberPathSegments(int i)  { return (alpha_ < 0.5 * M_PI)?((i==0)?2:((i==1)?3:((i==2)?3:1))):9; }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   // double a_;
   // double alpha_;

   // There are two possible shapes for the Wigner-Seitz cell:
   // rhombic dodecahedron when alpha < pi/2, trucated octahedron
   // when alpha > pi/2. The following arrays will be sized to
   // accomodate the larger case.
   //
   // | type      |num nodes | num elems | num bdr elems |
   // |-----------|----------|-----------|---------------|
   // | rhomb-dod |    14    |     4     |      12       |
   // | trunc-oct |    38    |    16     |      30       |
   //
   void createRhombicDodecahedron();
   void createTruncatedOctahedron();

   double ws_vert_[114];  // Vertex coordinates
   int ws_e2v_[128];      // Element to vertex connectivity
   int ws_elem_att_[16];  // Element Attributes
   int ws_be2v_[120];     // Boundary Element to vertex connectivity
   int ws_belem_att_[30]; // Boundary element Attributes
};

class MonoclinicLattice : public BravaisLattice3D
{
public:
   MonoclinicLattice(double a = 1.0, double b = 1.0, double c = 1.2,
                     double alpha = 0.4*M_PI);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()     { return 16; }
   virtual unsigned int GetNumberIntermediatePoints() { return 11; }
   virtual unsigned int GetNumberPaths()              { return 3; }
   virtual unsigned int GetNumberPathSegments(int i)
   { return (i==0)?8:((i==1)?2:1); }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   // double a_;
   // double b_;
   // double c_;
   // double alpha_;

   // Data for mesh of the corresponding Wigner-Setiz Cell
   double ws_vert_[63];   // Vertex coordinates
   int ws_e2v_[48];       // Element to vertex connectivity
   int ws_elem_att_[6];   // Element Attributes
   int ws_be2v_[72];      // Boundary Element to vertex connectivity
   int ws_belem_att_[18]; // Boundary element Attributes
};

/**

Truncated Octahderon for a < b < c and  b^2 - a^2 < 2 b c cos(alpha) < b^2 + a^2

Elongated Dodecahedron for a < b < c  and 2 b c cos(alpha) < b^2 - a^2

Truncated Octahderon for b < a < c and  c cos(alpha) < b

*/
class BaseCenteredMonoclinicLattice : public BravaisLattice3D
{
public:
   BaseCenteredMonoclinicLattice(double a = 1.0, double b = 1.0, double c = 1.2,
                                 double alpha = 0.4*M_PI);

   virtual bool MapToFundamentalDomain(const Vector & pt,
                                       Vector & ipt) const;

   virtual unsigned int GetNumberSymmetryPoints()     { return 16; }
   virtual unsigned int GetNumberIntermediatePoints() { return 11; }
   virtual unsigned int GetNumberPaths()              { return 3; }
   virtual unsigned int GetNumberPathSegments(int i)
   { return (i==0)?8:((i==1)?2:1); }

   mfem::Mesh * GetCoarseWignerSeitzMesh() const;

private:
   // double a_;
   // double b_;
   // double c_;
   // double alpha_;

   void createElongatedDodecahedron();
   void createTruncatedOctahedron();

   // Data for mesh of the corresponding Wigner-Setiz Cell
   double ws_vert_[114];  // Vertex coordinates
   int ws_e2v_[128];      // Element to vertex connectivity
   int ws_elem_att_[16];  // Element Attributes
   int ws_be2v_[120];     // Boundary Element to vertex connectivity
   int ws_belem_att_[30]; // Boundary element Attributes
};

/// Factory function to construct and return a pointer to a specific
/// type of BravaisLattice object.  Most lattice types will ignore
/// some of the parameters which are not relevant to them.  If any of
/// the parameters are negative they will be replaced with default
/// values.
BravaisLattice *
BravaisLatticeFactory(BRAVAIS_LATTICE_TYPE type,
                      double a, double b, double c,
                      double alpha, double beta, double gamma,
                      int logging = 0);

/// ModeCoefficient is an abstract base class for scalar coefficients
/// which implements Fourier modes in 3D.  The modes are indexed using
/// three integers which can take any positive or negative value.  The
/// integers are used to form a linear combination of reciprocal
/// vectors of a Bravais unit cell which is the wave vector of the
/// mode.  Specifically, this can be written as:
///    a * exp( i * (n0 b0 + n1 b1 + n2 b2).(x,y,z) )
/// Where the b_i are the reciprocal lattice vectors.
///
class ModeCoefficient : public Coefficient
{
public:
   ModeCoefficient() : n0_(0), n1_(0), n2_(0), a_(1.0) {}

   void SetAmplitude(double a);
   void SetModeIndices(int n0, int n1 = 0, int n2 = 0);
   void SetReciprocalLatticeVectors(const std::vector<Vector> & rec_vecs);

   double Eval(ElementTransformation &T,
               const IntegrationPoint &ip);

protected:

   virtual double func_(double phase) const = 0;

private:
   int n0_, n1_, n2_;

   double a_;

   std::vector<Vector> rec_vecs_;
};

/// Computes the real part of:
///    a * exp( i * (n0 b0 + n1 b1 + n2 b2).(x,y,z) )
class RealModeCoefficient : public ModeCoefficient
{
public:
   RealModeCoefficient() {}

protected:
   double func_(double phase) const { return cos(phase); }
};

/// Computes the imaginary part of:
///    a * exp( i * (n0 b0 + n1 b1 + n2 b2).(x,y,z) )
class ImagModeCoefficient : public ModeCoefficient
{
public:
   ImagModeCoefficient() {}

protected:
   double func_(double phase) const { return sin(phase); }
};

class PhaseCoefficient : public Coefficient
{
public:
   PhaseCoefficient() : a_(1.0) {}

   void SetAmplitude(double a);
   void SetKappa(const Vector & kappa);

   double Eval(ElementTransformation &T,
               const IntegrationPoint &ip);

protected:

   virtual double func_(double phase) const = 0;

private:
   double a_;
   Vector kappa_;
};

class RealPhaseCoefficient : public PhaseCoefficient
{
public:
   RealPhaseCoefficient() {}

protected:
   double func_(double phase) const { return cos(phase); }
};

class ImagPhaseCoefficient : public PhaseCoefficient
{
public:
   ImagPhaseCoefficient() {}

protected:
   double func_(double phase) const { return sin(phase); }
};

class BravaisFourierSeries
{
public:
   virtual ~BravaisFourierSeries();

   void SetMode(int n0 = 0, int n1 = 0, int n2 = 0);

   // int GetNMax() const { return n_; }

   // void SetNMax(int n) { n_ = n; }

protected:

   BravaisFourierSeries(const BravaisLattice & bravais,
                        ParFiniteElementSpace & fes);

   virtual void init() = 0;

   ParLinearForm         * br_;
   ParLinearForm         * bi_;

   RealModeCoefficient coefr_;
   ImagModeCoefficient coefi_;
   std::vector<Vector>      rec_vecs_;
   int n0_, n1_, n2_;
   double vol_;
};

class ScalarFourierSeries : public BravaisFourierSeries
{
public:
   ~ScalarFourierSeries();

   void GetCoefficient(HypreParVector & v,
                       double & a_r, double & a_i);

protected:
   ScalarFourierSeries(const BravaisLattice & bravais,
                       ParFiniteElementSpace & fes);

   void init();

   HypreParVector * Br_;
   HypreParVector * Bi_;
};

class VectorFourierSeries : public BravaisFourierSeries
{
public:
   ~VectorFourierSeries();

   void GetCoefficient(HypreParVector & v,
                       Vector & a_r, Vector & a_i);

protected:
   VectorFourierSeries(const BravaisLattice & bravais,
                       ParFiniteElementSpace & fes);

   void init();

   Vector vec_;
   VectorFunctionCoefficient vecCoef_;

   HypreParVector * Br_[3];
   HypreParVector * Bi_[3];
};

class H1FourierSeries : public ScalarFourierSeries
{
public:
   H1FourierSeries(const BravaisLattice & bravais,
                   mfem::miniapps::H1_ParFESpace & fes);
private:
};

class L2FourierSeries : public ScalarFourierSeries
{
public:
   L2FourierSeries(const BravaisLattice & bravais,
                   mfem::miniapps::L2_ParFESpace & fes);
private:
};

class HCurlFourierSeries : public VectorFourierSeries
{
public:
   HCurlFourierSeries(const BravaisLattice & bravais,
                      mfem::miniapps::ND_ParFESpace & fes);
};

class HDivFourierSeries : public VectorFourierSeries
{
public:
   HDivFourierSeries(const BravaisLattice & bravais,
                     mfem::miniapps::RT_ParFESpace & fes);
};


Mesh *
MakePeriodicMesh(Mesh * mesh, const std::vector<Vector> & trans_vecs,
                 int logging = 0);

class LatticeCoefficient : public Coefficient
{
public:
   LatticeCoefficient(const BravaisLattice & bl, double frac = 0.5,
                      double val0 = 0.0, double val1 = 1.0);

   double Eval(ElementTransformation &T,
               const IntegrationPoint &ip);

private:
   std::vector<Vector> axes_;
   std::vector<double> radii_;
   double frac_;
   double a0_;
   double a1_;

   mutable Vector x_;
   mutable Vector xp_;
};

} // namespace bravais
} // namespace mfem

#endif // MFEM_BRAVAIS
