cmake_minimum_required(VERSION 3.10)
project(gadget)

add_executable(NGenIC
        main.c power.c allvars.c save.c read_param.c  read_glass.c)
target_link_libraries(NGenIC
        hdf5::hdf5
        MPI::MPI_C gsl gslcblas m
)
target_compile_definitions(NGenIC PRIVATE  HAVE_HDF5)
if (NOTYPEPREFIX_FFTW)
    target_link_libraries(NGenIC rfftw_mpi fftw_mpi rfftw fftw)
elseif(DOUBLEPRECISION_FFTW)
    target_link_libraries(NGenIC drfftw_mpi dfftw_mpi drfftw dfftw)
else()
    # target_link_libraries(NGenIC srfftw_mpi sfftw_mpi srfftw sfftw)
    target_link_libraries(NGenIC fftw3 fftw3_mpi)
endif()

# Install gadget
install(TARGETS NGenIC DESTINATION bin)