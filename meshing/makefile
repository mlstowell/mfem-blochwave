# Copyright (c) 2010, Lawrence Livermore National Security, LLC. Produced at the
# Lawrence Livermore National Laboratory. LLNL-CODE-443211. All Rights reserved.
# See file COPYRIGHT for details.
#
# This file is part of the MFEM library. For more information and source code
# availability see http://mfem.org.
#
# MFEM is free software; you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License (as published by the Free
# Software Foundation) version 2.1 dated February 1999.

# Use the MFEM build directory
BRAVAIS_DIR ?= ..
BRAVAIS_BUILD_DIR ?= ..
SRC = $(if $(BRAVAIS_DIR:..=),$(BRAVAIS_DIR)/meshing/,)
CONFIG_MK = $(MFEM_BUILD_DIR)/config/config.mk
# Use the MFEM install directory
# MFEM_DIR = ../../mfem
# CONFIG_MK = $(MFEM_DIR)/config.mk
# TEST_MK = $(MFEM_DIR)/test.mk

BRAVAIS_LIB_FILE = bravais_is_not_built
-include $(CONFIG_MK)

SEQ_MINIAPPS = toct mobius-strip klein-bottle mesh-explorer
PAR_MINIAPPS =
ifeq ($(MFEM_USE_MPI),NO)
   MINIAPPS = $(SEQ_MINIAPPS)
else
   MINIAPPS = $(PAR_MINIAPPS) $(SEQ_MINIAPPS)
endif

.SUFFIXES:
.SUFFIXES: .o .cpp .mk
.PHONY: all clean clean-build clean-exec

# Remove built-in rule
%: %.cpp

# Replace the default implicit rule for *.cpp files
%: $(SRC)%.cpp $(MFEM_LIB_FILE) $(CONFIG_MK)
	$(MFEM_CXX) $(MFEM_FLAGS) $< -o $@ $(MFEM_LIBS)

periodic-bravais-mesh: periodic-bravais-mesh.cpp $(BRAVAIS_LIB_FILE) $(CONFIG_MK)
	$(MFEM_CXX) $(MFEM_FLAGS) $< -o $@ -I../lib $(MFEM_LIBS) -L .. -lmfem-bravais

all: $(MINIAPPS)

#MFEM_TESTS = MINIAPPS
#include $(TEST_MK)

# Testing: Parallel vs. serial runs
RUN_MPI = $(MFEM_MPIEXEC) $(MFEM_MPIEXEC_NP) 4
%-test-par: %
	@$(call mfem-test-file,$<, $(RUN_MPI), Meshing miniapp,$(<).mesh)
%-test-seq: %
	@$(call mfem-test-file,$<,, Meshing miniapp,$(<).mesh)

# Testing: Specific execution options
mesh-explorer-test-seq:
	@true

# Testing: "test" target and mfem-test* variables are defined in config/test.mk

# Generate an error message if the MFEM library is not built and exit
$(MFEM_LIB_FILE):
	$(error The MFEM library is not built)

clean: clean-build clean-exec

clean-build:
	rm -f *.o *~ mobius-strip klein-bottle mesh-explorer
	rm -rf *.dSYM *.TVD.*breakpoints

clean-exec:
	@rm -f mobius-strip.mesh klein-bottle.mesh mesh-explorer.mesh
	@rm -f partitioning.txt
