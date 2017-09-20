# Colored Abstract Simplicial Complex (CASC) Library

CASC is a header-only C++ library which provides a data structure to represent arbitrary dimension abstract simplicial complexes.
It also allows for user-defined classes to be stored on simplices at each dimension. This library is released as a part of the Finite Elements Toolkit ([FETK](http://www.fetk.org/))


## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

To use this library you need to have a C++ compiler which supports C++14.

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
```

```
make 
make install
```

In order to build the documentation:

```
make casc_doc
```

## Versioning

We use [Bitbucket](https://bitbucket.org/fetk/casc) for versioning. For the versions available, see the [tags on this repository](https://bitbucket.org/fetk/casc/downloads/?tab=tags). 

## Authors

* **John Moody** <jbmoody@ucsd.edu>
* **Christopher Lee** <ctlee@ucsd.edu>

## License

This project is licensed under the ... License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

This project is supported by grants...