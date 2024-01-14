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
        ${HDF5_C_LIBRARIES} ${HDF5_HL_LIBRARIES}
        MPI::MPI_C gsl gslcblas m
)
if (NOTYPEPREFIX_FFTW)
    target_link_libraries(Gadget2 rfftw_mpi fftw_mpi rfftw fftw)
elseif(DOUBLEPRECISION_FFTW)
    target_link_libraries(Gadget2 drfftw_mpi dfftw_mpi drfftw dfftw)
else()
    # target_link_libraries(Gadget2 srfftw_mpi sfftw_mpi srfftw sfftw)
    target_link_libraries(Gadget2 fftw3 fftw3_mpi)
endif()

if (PERIODIC)
    target_compile_definitions(Gadget2 PRIVATE PERIODIC)
endif()
if (UNEQUALSOFTENINGS)
    target_compile_definitions(Gadget2 PRIVATE UNEQUALSOFTENINGS)
endif()
if (PEANOHILBERT)
    target_compile_definitions(Gadget2 PRIVATE PEANOHILBERT)
endif()
if (WALLCLOCK)
    target_compile_definitions(Gadget2 PRIVATE WALLCLOCK)
endif()
if (SYNCHRONIZATION)
    target_compile_definitions(Gadget2 PRIVATE SYNCHRONIZATION)
endif()

install(TARGETS Gadget2 DESTINATION bin)