# Building the documentation

The documentation for CASC can be generated locally using 
[Doxygen](http://www.stack.nl/~dimitri/doxygen/). You must have a working copy
of doxygen installed on your machine in order to build the documentation.

If CMake is able to find your doxygen installation then the following sequence
of commands will build the basic documentation.
```
cmake ..
make docs
```

### Documentation for Developers

If you are contributing to or modifying the CASC library you may wish to
document private class members or currently hidden metatemplate helper functions.
Whether or not documentation for these items is generated can be controlled by
modifying the default doxygen configuration: `doc/Doxyfile.in`. 

To document private class functions and members toggle:
`EXTRACT_PRIVATE = YES`

To enable metatemplate helper functions enable the conditional:
`ENABLED_SECTIONS = detail`
