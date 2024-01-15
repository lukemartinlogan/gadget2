cmake_minimum_required(VERSION 3.10)
project(gadget)

add_executable(Gadget2
        main.c  run.c  predict.c begrun.c endrun.c global.c
        timestep.c  init.c restart.c  io.c
        accel.c   read_ic.c  ngb.c
        system.c  allocate.c  density.c
        gravtree.c hydra.c  driftfac.c
        domain.c  allvars.c potential.c
        forcetree.c   peano.c gravtree_forcetest.c
        pm_periodic.c pm_nonperiodic.c longrange.c )
target_link_libraries(Gadget2
        hdf5::hdf5
        MPI::MPI_C gsl gslcblas m
)
if (NOTYPEPREFIX_FFTW)
    target_link_libraries(Gadget2 rfftw_mpi fftw_mpi rfftw fftw)
elseif(DOUBLEPRECISION_FFTW)
    target_link_libraries(Gadget2 drfftw_mpi dfftw_mpi drfftw dfftw)
else()
    target_link_libraries(Gadget2 srfftw_mpi sfftw_mpi srfftw sfftw)
    # target_link_libraries(Gadget2 fftw3 fftw3_mpi)
endif()

# Various compile-time options
#--------------------------------------- Basic operation mode of code
if (PERIODIC)
    target_compile_definitions(Gadget2 PRIVATE PERIODIC)
endif()
if (UNEQUALSOFTENINGS)
    target_compile_definitions(Gadget2 PRIVATE UNEQUALSOFTENINGS)
endif()
#--------------------------------------- Things that are always recommended
if (PEANOHILBERT)
    target_compile_definitions(Gadget2 PRIVATE PEANOHILBERT)
endif()
if (WALLCLOCK)
    target_compile_definitions(Gadget2 PRIVATE WALLCLOCK)
endif()
#--------------------------------------- TreePM Options
#--------------------------------------- Single/Double Precision
if (DOUBLEPRECISION)
    target_compile_definitions(Gadget2 PRIVATE DOUBLEPRECISION)
endif()
if (DOUBLEPRECISION_FFTW)
    target_compile_definitions(Gadget2 PRIVATE DOUBLEPRECISION_FFTW)
endif()
#--------------------------------------- Time integration options
if (SYNCHRONIZATION)
    target_compile_definitions(Gadget2 PRIVATE SYNCHRONIZATION)
endif()

if (FLEXSTEPS)
    target_compile_definitions(Gadget2 PRIVATE FLEXSTEPS)
endif()
if (PSEUDOSYMMETRIC)
    target_compile_definitions(Gadget2 PRIVATE PSEUDOSYMMETRIC)
endif()
if (NOSTOP_WHEN_BELOW_MINTIMESTEP)
    target_compile_definitions(Gadget2 PRIVATE NOSTOP_WHEN_BELOW_MINTIMESTEP)
endif()
if (NOPMSTEPADJUSTMENT)
    target_compile_definitions(Gadget2 PRIVATE NOPMSTEPADJUSTMENT)
endif()
#--------------------------------------- Output options
target_compile_definitions(Gadget2 PRIVATE  HAVE_HDF5)
#--------------------------------------- Things for special behaviour
if (NOGRAVITY)
    target_compile_definitions(Gadget2 PRIVATE NOGRAVITY)
endif()
#--------------------------------------- Glass making

# Install gadget
install(TARGETS Gadget2 DESTINATION bin)