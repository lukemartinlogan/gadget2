
  *************************************************************
  *  GADGET-2   -  A code for collisionless and gasdynamical  *
  *                cosmological simulations                   *
  *                                                           *
  *        (GAlaxies with Dark matter and GasintEracT)        *
  *         --            -               -     -   -         *
  *        	                                              *
  *  written by:   Volker Springel, MPA, (c) 2005             *
  *        	                                              *
  *************************************************************

This repo is a fork of the Gadget2 code -- specifically Gadget 2.0.7.
It was downloaded from [here](https://wwwmpa.mpa-garching.mpg.de/gadget/).
This repo contains changes to the build system of the original code to
be more modern.

Specifically:
* Support for recent HDF5 versions
* CMake for the build system instead of make
* Includes the initial condition boundary code [N_GeenIC](https://wwwmpa.mpa-garching.mpg.de/gadget/n-genic.tar.gz),
also built as apart of the CMake

Install dependencies with spack
```
spack install hdf5@1.14.1 gsl@2.1 fftw@2
spack load hdf5@1.14.1 gsl@2.1 fftw@2
```

# N-GenIC

The dummy glass file contains a grid of 4096 particles. The initial condition
code ics.param will expand this to be 4096 * 8^3 partciles, where 8 is TileFac.

```
mkdir build
cd build
cmake ../ -DFFTW_PATH=$(spack find --format "{PREFIX}" fftw@2)
# NOTE: fftw@2 does not have a nice integration with cmake
cd ../N-GenIC
mpirun -n 2 ../build/bin/NGenIC ics.param
```

# Gassphere
```
mkdir build
cd build
cmake ../ \
-DPEANOHILBERT=ON \
-DWALLCLOCK=ON \
-DSYNCHRONIZATION=ON
make -j8
cd ../
mkdir gassphere
build/bin/Gadget2 Gadget2/parameterfiles/gassphere.param
```

  Please see the files in the directory `Documentation' for a 
  detailed discription of this simulation software. There you 
  should find the following files:

  gadget2-paper.pdf - Discusses algorithmic aspects of the code

  users-guide.pdf - Instructions for code usage

  In addition, please use your browser to access the 
  cross-referenced documentation of the source code, contained 
  in the `html/' subdirectory (open the `index.html' file).
