# Colored Abstract Simplicial Complex (CASC) Library
[![DOI](https://zenodo.org/badge/121550288.svg)](https://zenodo.org/badge/latestdoi/121550288)
[![Main Build Status](https://github.com/ctlee/casc/actions/workflows/ci.yaml/badge.svg?branch=master)](https://github.com/ctlee/casc/actions/workflows/ci.yaml) 

CASC is a modern and header-only C++ library which provides a data structure 
to represent arbitrary dimension abstract simplicial complexes with user-defined classes stored directly on the simplices at each dimension.
This is achieved by taking advantage of the combinatorial nature of simplicial complexes and new C++ code features such as: variadic templates and automatic function return type deduction.
Essentially CASC stores the full topology of the complex according to a [Hasse diagram](https://en.wikipedia.org/wiki/Hasse_diagram).
The representation of the topology is decoupled from interactions of user data through the use of metatemplate programming.

## Getting Started
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites
CASC does not have any dependencies other than the C++ standard library.
If you wish to use CASC, you can use the header files right away.
There is no binary library to link to, and no configured header file.
CASC is a pure template library defined in the headers.

We use the CMake build system (version 3+), but only to build the documentation and unit-tests, and to automate installation. 

Doxygen and Graphviz are used to generate the documentation.

To use CASC in your software all you will need is a working C++ compiler with full C++14 support.
This includes:
- GCC Versions 5+
- Clang Versions 3, 5+<sup>†</sup>
- XCode 8+<sup>†</sup>

<sup>†</sup> Note that there is a known issue with Clang 4.x.x versioned compilers (including XCode version 9.[0-2]), where the most specialized unique specialization is not selected leading to a compiler error. 
The current workaround to this problem is to either use GCC or to obtain Clang version 5+ (XCode version 9.3beta+).

### Installing
CASC is header only meaning that there is nothing to compile out of the box.
To use CASC, simply copy the desired headers into your project and included as necessary.
If you wish to install CASC using CMake to your system, even though the library is header only, you must first create a new folder to prevent in-source "builds".
```bash
mkdir build
cd build
```
Subsequently run CMake specifying the installation prefix and the path to the root level `CMakeLists.txt` file. 
```bash 
cmake -DCMAKE_INSTALL_PREFIX=/usr/local/ ..
make install
```
Unit tests are also packaged along with CASC and are dependent upon [Googles C++ test framework](https://github.com/google/googletest).
If you wish to build and run the tests, set the flag `-DBUILD_CASCTESTS=on` in your CMake command.
CMake will then download and build `googletest` and link it with the CASC unit tests.
```bash
cmake -DBUILD_CASCTESTS=on ..
make
make tests  		# Run tests through make
./bin/casctests 	# Alternatively run the tests directly (more verbose)
```

### Documentation
A current version of the documentation is available online via [github pages](https://ctlee.github.io/casc). 
You can also build the documentation locally if you have [Doxygen](http://www.stack.nl/~dimitri/doxygen/) and Graphviz on your system.
CMake will automatically try to find a working Doxygen installation.
If CMake is able to find your doxygen installation then the following sequence
of commands will build the basic documentation.

```
cmake ..
make docs
```

#### Documentation for Developers

If you are contributing to or modifying the CASC library you may wish to
document private class members or currently hidden metatemplate helper functions.
Configure the repository with this extra cmake option to expose the details before building the documentation.

```
-DCASC_DOCS_ADVANCED=ON
```
## Versioning & Contributing
We use [Github](https://github.com/ctlee/CASC) for versioning. 
For the versions available, please see the [releases](https://github.com/ctlee/CASC/releases/). 
If you find a bug or with to request additional functionality please file an issue in the [CASC Github project](https://github.com/ctlee/CASC/issues).

## Authors
**John Moody**  
Department of Mathematics  
University of California, San Diego  

**[Christopher T. Lee](https://github.com/ctlee)**  
Department of Chemistry & Biochemistry  
University of California, San Diego  

See also the list of [contributors](https://github.com/ctlee/CASC/contributors) who participated in this project.

Please cite the above Zenodo DOI to acknowledge the software version and cite the following paper:<br/>
[Lee, C. T.; Moody, J. B.; Amaro, R. E.; Mccammon, J. A.; Holst, M. J. The Implementation of the Colored Abstract Simplicial Complex and Its Application to Mesh Generation. ACM Trans. Math. Softw. 2019, 45 (3), 1–20.](https://doi.org/10.1145/3321515)

## License
This project is licensed under the GNU Lesser General Public License v2.1 - 
please see the [COPYING.md](COPYING.md) file for details.

## Acknowledgments
This project is supported by the National Institutes of Health under grant numbers P41-GM103426 ([NBCR](http://nbcr.ucsd.edu/)), T32-GM008326, and R01-GM31749. 
It is also supported in part by the National Science Foundation under awards DMS-CM1620366 and DMS-FRG1262982.

## Development Build Status

[![Development Build Status](https://github.com/ctlee/casc/actions/workflows/ci.yaml/badge.svg?branch=development)](https://github.com/ctlee/casc/actions/workflows/ci.yaml)
