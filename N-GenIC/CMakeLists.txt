cmake_minimum_required(VERSION 3.10)
project(gadget)

add_executable(NGenIC
        main.c power.c allvars.c save.c read_param.c  read_glass.c)
set (NGenIC_Libs
        hdf5::hdf5
        MPI::MPI_C gsl gslcblas m)
target_compile_definitions(NGenIC PRIVATE  HAVE_HDF5)
if (NOTYPEPREFIX_FFTW)
    set (NGenIC_Libs
            rfftw_mpi fftw_mpi rfftw fftw ${NGenIC_Libs})
elseif(DOUBLEPRECISION_FFTW)
    set (NGenIC_Libs
            drfftw_mpi dfftw_mpi drfftw dfftw ${NGenIC_Libs})
else()
    set (NGenIC_Libs
            srfftw_mpi sfftw_mpi srfftw sfftw ${NGenIC_Libs})
endif()
target_link_libraries(NGenIC ${NGenIC_Libs})

target_compile_definitions(NGenIC PRIVATE  NO64BITID)
if (CORRECT_CIC)
    target_compile_definitions(NGenIC PRIVATE  CORRECT_CIC)
endif()
if (PRODUCEGAS)
    target_compile_definitions(NGenIC PRIVATE  PRODUCEGAS)
endif()

add_executable(make_dummy_glass make_dummy_glass.cc)
target_link_libraries(make_dummy_glass ${NGenIC_Libs})

# Install gadget
install(TARGETS
        NGenIC
        make_dummy_glass
        DESTINATION bin)