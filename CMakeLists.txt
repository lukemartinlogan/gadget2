cmake_minimum_required(VERSION 3.10)
project(gadget)

#-----------------------------------------------------------------------------
# Define Options
#-----------------------------------------------------------------------------
option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" ON)
option(BUILD_MPI_TESTS "Build tests which depend on MPI" ON)
option(BUILD_OpenMP_TESTS "Build tests which depend on OpenMP" ON)
set(FFTW_PATH "" CACHE STRING "Path to FFTW installation")
include_directories(${FFTW_PATH}/include)
link_directories(${FFTW_PATH}/lib)

# - PERIODIC:
#     Set this if you want to have periodic boundary conditions.
option(PERIODIC "PERIODIC" OFF)

# - UNEQUALSOFTENINGS:
#     Set this if you use particles with different gravitational
#     softening lengths.
option(UNEQUALSOFTENINGS "UNEQUALSOFTENINGS" OFF)

# - PEANOHILBERT:
#     This is a tuning option. When set, the code will bring the
#     particles after each domain decomposition into Peano-Hilbert
#     order. This improves cache utilization and performance.
option(PEANOHILBERT "PEANOHILBERT" OFF)

# - WALLCLOCK:
#     If set, a wallclock timer is used by the code to measure internal
#     time consumption (see cpu-log file).  Otherwise, a timer that
#     measures consumed processor ticks is used.
option(WALLCLOCK "WALLCLOCK" OFF)

# - PMGRID:
#     This enables the TreePM method, i.e. the long-range force is
#     computed with a PM-algorithm, and the short range force with the
#     tree. The parameter has to be set to the size of the mesh that
#     should be used, (e.g. 64, 96, 128, etc). The mesh dimensions need
#     not necessarily be a power of two.  Note: If the simulation is
#     not in a periodic box, then a FFT method for vacuum boundaries is
#     employed, using an actual mesh with dimension twice(!) that
#     specified by PMGRID.
set(PMGRID OFF CACHE STRING "PMGRID")

# - PLACEHIGHRESREGION:
#     If this option is set (will only work together with PMGRID), then
#     the long range force is computed in two stages: One Fourier-grid
#     is used to cover the whole simulation volume, allowing the
#     computation of the longe-range force.  A second Fourier mesh is
#     placed on the region occupied by "high-resolution" particles,
#     allowing the computation of an intermediate scale force. Finally,
#     the force on short scales is computed with the tree. This
#     procedure can be useful for "zoom-simulations", provided the
#     majority of particles (the high-res particles) are occupying only
#     a small fraction of the volume. To activate this option, the
#     parameter needs to be set to an integer bit mask that encodes the
#     particle types that make up the high-res particles.
#     For example, if types 0, 1, and 4 form the high-res
#     particles, set the parameter to PLACEHIGHRESREGION=19, because
#     2^0 + 2^1 + 2^4 = 19. The spatial region covered by the high-res
#     grid is determined automatically from the initial conditions.
#     Note: If a periodic box is used, the high-res zone may not intersect
#     the box boundaries.
option(PLACEHIGHRESREGION "PLACEHIGHRESREGION" OFF)

# - ENLARGEREGION:
#     The spatial region covered by the high-res zone has a fixed size
#     during the simulation, which initially is set to the smallest
#     region that encompasses all high-res particles. Normally, the
#     simulation will be interrupted if high-res particles leave this
#     region in the course of the run. However, by setting this
#     parameter to a value larger than one, the size of the high-res
#     region can be expanded, providing a buffer region.  For example,
#     setting it to 1.4 will enlarge its side-length by 40% (it remains
#     centered on the high-res particles). Hence, with this setting, the
#     high-res region may expand or move by a limited amount.
#     Note: If SYNCHRONIZATION is activated, the code will be able to
#     continue even if high-res particles leave the initial high-res
#     grid. In this case, the code will update the size and position of
#     the grid that is placed onto the high-resolution region
#     automatically. To prevent that this potentially happens every
#     single PM step, one should nevertheless assign a value slightly
#     larger than 1 to ENLARGEREGION.
option(ENLARGEREGION "ENLARGEREGION" OFF)

# - ASMTH:
#     This can be used to override the value assumed for the scale that
#     defines the long-range/short-range force-split in the TreePM
#     algorithm. The default value is 1.25, in mesh-cells.
option(ASMTH "ASMTH" OFF)

# - RCUT:
#     This can be used to override the maximum radius in which the
#     short-range tree-force is evaluated (in case the TreePM algorithm
#     is used). The default value is 4.5, given in mesh-cells.
option(RCUT "RCUT" OFF)

# - DOUBLEPRECISION:
#     This makes the code store and compute internal particle data in
#     double precision. Note that output files are nevertheless written
#     by converting the particle data to single precision.
option(DOUBLEPRECISION "DOUBLEPRECISION" OFF)

# - DDOUBLEPRECISION_FFTW:
#     If this is set, the code will use the double-precision version of
#     FTTW, provided the latter has been explicitly installed with a
#     "d" prefix, and NOTYPEPREFIX_FFTW is not set. Otherwise the
#     single precision version ("s" prefix) is used.
option(DDOUBLEPRECISION_FFTW "DDOUBLEPRECISION_FFTW" OFF)

# - SYNCHRONIZATION:
#     When this is set, particles are kept in a binary hierarchy of
#     timesteps and may only increase their timestep if the new
#     timestep will put them into synchronization with the higher time
#     level.
option(SYNCHRONIZATION "SYNCHRONIZATION" OFF)

# - FLEXSTEPS:
#     This is an alternative to SYNCHRONIZATION. Particle timesteps are
#     here allowed to be integer multiples of the minimum timestep that
#     occurs among the particles, which in turn is rounded down to the
#     nearest power-of-two devision of the total simulated
#     timespan. This option distributes particles more evenly over
#     individual system timesteps, particularly once a simulation has
#     run for a while, and may then result in a reduction of work-load
#     imbalance losses.
option(FLEXSTEPS "FLEXSTEPS" OFF)

# - PSEUDOSYMMETRIC:
#     When this option is set, the code will try to "anticipate"
#     timestep changes by extrapolating the change of the acceleration
#     into the future. This can in certain idealized cases improve the
#     long-term integration behaviour of periodic orbits, but should
#     make little or no difference in most real-world applications. May
#     only be used together with SYNCHRONIZATION.
option(PSEUDOSYMMETRIC "PSEUDOSYMMETRIC" OFF)

# - NOSTOP_WHEN_BELOW_MINTIMESTEP:
#     If this is activated, the code will not terminate when the
#     timestep falls below the value of MinSizeTimestep specified in
#     the parameterfile. This is useful for runs where one wants to
#     enforce a constant timestep for all particles. This can be done
#     by activating this option, and by setting MinSizeTimestep and
#     MaxSizeTimestep to an equal value.
option(NOSTOP_WHEN_BELOW_MINTIMESTEP "NOSTOP_WHEN_BELOW_MINTIMESTEP" OFF)

# - NOPMSTEPADJUSTMENT:
#     When this is set, the long-range timestep for the PM-force
#     computation (when the TreePM algorithm is used) is always
#     determined by MaxSizeTimeStep.  Otherwise, it is determined by
#     the MaxRMSDisplacement parameter, or MaxSizeTimeStep, whichever
#     gives the smaller step.
option(NOPMSTEPADJUSTMENT "NOPMSTEPADJUSTMENT" OFF)

# - HAVE_HDF5:
#     If this is set, the code will be compiled with support for input
#     and output in the HDF5 format. You need to have the HDF5
#     libraries and headers installed on your computer for this option
#     to work. The HDF5 format can then be selected as format "3" in
#     Gadget's parameterfile.
option(HAVE_HDF5 "HAVE_HDF5" ON)

# - OUTPUTPOTENTIAL:
#     This will make the code compute gravitational potentials for
#     all particles each time a snapshot file is generated. The values
#     are then included in the snapshot file. Note that the computation
#     of the values of the gravitational potential costs additional CPU.
option(OUTPUTPOTENTIAL "OUTPUTPOTENTIAL" OFF)

# - OUTPUTACCELERATION:
#     This will include the physical acceleration of each particle in
#     snapshot files.
option(OUTPUTACCELERATION "OUTPUTACCELERATION" OFF)

# - OUTPUTCHANGEOFENTROPY:
#     This will include the rate of change of entropy of gas particles
#     in snapshot files.
option(OUTPUTCHANGEOFENTROPY "OUTPUTCHANGEOFENTROPY" OFF)

# - OUTPUTTIMESTEP:
#     This will include the current timesteps of all particles in the
#     snapshot files.
option(OUTPUTTIMESTEP "OUTPUTTIMESTEP" OFF)

# - NOGRAVITY
#     This switches off gravity. Useful only for pure SPH simulations
#     in non-expanding space.
option(NO_GRAVITY "NO_GRAVITY" OFF)

# - NOTREERND:
#     If this is not set, the tree construction will succeed even when
#     there are a few particles at identical locations. This is done by
#     `rerouting' particles once the node-size has fallen below 1.0e-3
#     of the softening length. When this option is activated, this will
#     be surpressed and the tree construction will always fail if there
#     are particles at extremely close coordinates.
option(NOTREERND "NOTREERND" OFF)

# - NOTYPEPREFIX_FFTW:
#     This is an option that signals that FFTW has been compiled
#     without the type-prefix option, i.e. no leading "d" or "s"
#     characters are used to access the library.
option(NOTYPEPREFIX_FFTW "NOTYPEPREFIX_FFTW" OFF)

# - LONG_X/Y/Z:
#     These options can be used together with PERIODIC and NOGRAVITY only.
#     When set, the options define numerical factors that can be used to
#     distorts the periodic simulation cube into a parallelepiped of
#     arbitrary aspect ratio. This can be useful for idealized SPH tests.
option(LONG_X "LONG_X" OFF)

# - TWODIMS:
#     This effectively switches of one dimension in SPH, i.e. the code
#     follows only 2d hydrodynamics in the xy-, yz-, or xz-plane. This
#     only works with NOGRAVITY, and if all coordinates of the third
#     axis are exactly equal. Can be useful for idealized SPH tests.
option(TWODIMS "TWODIMS" OFF)

# - SPH_BND_PARTICLES:
#     If this is set, particles with a particle-ID equal to zero do not
#     receive any SPH acceleration. This can be useful for idealized
#     SPH tests, where these particles represent fixed "walls".
option(SPH_BND_PARTICLES "SPH_BND_PARTICLES" OFF)

# - NOVISCOSITYLIMITER:
#     If this is set, the code will not try to put an upper limit on
#     the viscous force in case an implausibly high pair-wise viscous
#     force (which may lead to a particle 'reflection' in case of poor
#     timestepping) should arise. Note: For proper settings of the
#     timestep parameters, this situation should not arise.
option(NOVISCOSITYLIMITER "NOVISCOSITYLIMITER" OFF)

# - COMPUTE_POTENTIAL_ENERGY:
#     When this option is set, the code will compute the gravitational
#     potential energy each time a global statistics is computed. This
#     can be useful for testing global energy conservation.
option(COMPUTE_POTENTIAL_ENERGY "COMPUTE_POTENTIAL_ENERGY" OFF)

# - LONGIDS:
#     If this is set, the code assumes that particle-IDs are stored as
#     64-bit long integers. This is only really needed if you want to
#     go beyond ~2 billion particles.
option(LONGIDS "LONGIDS" OFF)

# - ISOTHERM_EQS:
#     This special option makes the gas behave like an isothermal gas
#     with equation of state P = cs^2 * rho. The sound-speed cs is set by
#     the thermal energy per unit mass in the intial conditions,
#     i.e. cs^2=u. If the value for u is zero, then the initial gas
#     temperature in the parameter file is used to define the sound speed
#     according to cs^2 = 3/2 kT/mp, where mp is the proton mass.
option(ISOTHERM_EQS "ISOTHERM_EQS" OFF)

# - ADAPTIVE_GRAVSOFT_FORGAS:
#     When this option is set, the gravitational softening lengths used for
#     gas particles is tied to their SPH smoothing length. This can be useful
#     for dissipative collapse simulations. The option requires the setting
#     of UNEQUALSOFTENINGS.
option(ADAPTIVE_GRAVSOFT_FORGAS "ADAPTIVE_GRAVSOFT_FORGAS" OFF)

# - SELECTIVE_NO_GRAVITY:
#     This can be used for special computations where one wants to
#     exclude certain particle types from receiving gravitational
#     forces. The particle types that are excluded in this fashion are
#     specified by a bit mask, in the same as for the PLACEHIGHRESREGION
#     option.
option(SELECTIVE_NO_GRAVITY "SELECTIVE_NO_GRAVITY" OFF)

# - FORCETEST:
#     This can be set to check the force accuracy of the code. The
#     option needs to be set to a number between 0 and 1 (e.g. 0.01),
#     which is taken to specify a random fraction of particles for
#     which at each timestep forces by direct summation are
#     computed. The normal tree-forces and the correct direct
#     summation forces are collected in a file. Note that the
#     simulation itself is unaffected by this option, but it will of
#     course run much(!) slower, especially if
#     FORCETEST*NumPart*NumPart >> NumPart. Note: Particle IDs must
#     be set to numbers >=1 for this to work.
option(FORCETEST "FORCETEST" OFF)

# - MAKEGLASS
#     This option can be used to generate a glass-like particle
#     configuration. The value assigned gives the particle load,
#     which is initially generated as a Poisson sample and then
#     evolved towards a glass with the sign of gravity reversed.
option(MAKEGLASS "MAKEGLASS" OFF)

option(CORRECT_CIC "CORRECT_CIC" OFF)
option(PRODUCEGAS "PRODUCEGAS" OFF)

#-----------------------------------------------------------------------------
# Compiler Optimization
#-----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("IN DEBUG MODE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    add_compile_definitions(GADGET_LOG_VERBOSITY=10)
else()
    message("IN RELEASE MODE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O3")
    add_compile_definitions(GADGET_LOG_VERBOSITY=1)
endif()
add_compile_options(-march=native -fomit-frame-pointer)

#-----------------------------------------------------------------------------
# Find Packages
#-----------------------------------------------------------------------------
# This is for compatability with SPACK
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# MPI
find_package(MPI REQUIRED COMPONENTS C CXX)
message(STATUS "found mpi.h at ${MPI_CXX_INCLUDE_DIRS}")

# OpenMP
find_package(OpenMP REQUIRED COMPONENTS C CXX)
message(STATUS "found omp.h at ${OpenMP_CXX_INCLUDE_DIRS}")

# Pkg-Config
find_package(PkgConfig REQUIRED)
if(PkgConfig)
    message(STATUS "found pkg config")
endif()

# HDF5
set(GADGET_REQUIRED_HDF5_VERSION 1.14.0)
find_package(HDF5 ${GADGET_REQUIRED_HDF5_VERSION} REQUIRED)

# FFTW
#pkg_check_modules(fftw3 REQUIRED fftw3)
#include_directories(${fftw3_INCLUDE_DIRS})
#link_directories(${fftw3_LIBRARY_DIRS})

# GSL
pkg_check_modules(gsl REQUIRED gsl)
include_directories(${gsl_INCLUDE_DIRS})
link_directories(${gsl_LIBRARY_DIRS})

#------------------------------------------------------------------------------
# Setup CMake Environment
#------------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables.")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries.")

#------------------------------------------------------------------------------
# Setup install and output Directories
#------------------------------------------------------------------------------
if(NOT GADGET_INSTALL_BIN_DIR)
    set(GADGET_INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
endif()
if(NOT GADGET_INSTALL_LIB_DIR)
    set(GADGET_INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
endif()
if(NOT GADGET_INSTALL_INCLUDE_DIR)
    set(GADGET_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
endif()
if(NOT GADGET_INSTALL_DATA_DIR)
    set(GADGET_INSTALL_DATA_DIR ${CMAKE_INSTALL_PREFIX}/share)
endif()

#-----------------------------------------------------------------------------
# Build Hermes Main Packages
#-----------------------------------------------------------------------------
add_subdirectory(Gadget2)
add_subdirectory(N-GenIC)
