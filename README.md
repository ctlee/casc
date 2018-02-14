# Colored Abstract Simplicial Complex (CASC) Library
CASC is a header-only C++ library which provides a data structure to represent 
arbitrary dimension abstract simplicial complexes. It also allows for 
user-defined classes to be stored on simplices at each dimension. This library 
is released as a part of the Finite Elements Toolkit 
([FETK](http://www.fetk.org/)).

## Getting Started
These instructions will get you a copy of the project up and running on your 
local machine for development and testing purposes.

### Prerequisites
To use this library you need to have a C++ compiler which supports C++14. There
is a known issue with Clang 4.x.x versioned compilers (including XCode version 9)
, where the most specialized unique specialization is not selected leading to a 
compiler error. The current workaround to this problem 

### Installing
This library is header only. The desired headers can be included as necessary.
```
mkdir build
cd build
cmake -DBUILD_TESTS=ON ..
```
If you wish to build examples...
```
-DBUILD_EXAMPLES=ON 
make 
make install
```

In order to build the documentation:
```
make casc_doc
```

## Versioning

We use [Github](https://github.com/ctlee/CASC) for versioning. For the 
versions available, please see the [releases](https://github.com/ctlee/CASC/releases/). 

## Authors
* **John Moody** <jbmoody@ucsd.edu>
* **Christopher Lee** <ctlee@ucsd.edu>

## License
This project is licensed under the GNU Lesser General Public License v2.1 - 
please see the [COPYING.md](COPYING.md) file for details.

## Acknowledgments
This project is supported by the National Institutes of Health under grant
numbers P41-GM103426, T32-GM008326, and R01-GM31749. It's also supported in part
by the National Science Foundation under awards DMS-CM1620366 and DMS-FRG1262982.