# Greetings

Welcome to the installation!

**Contents**

  1. Requirements
  2. Installation of the commandline interface
  3. Installation of the Qt GUI
  4. Debugging

# 1 Requirements
You will need the following libraries, headers and tools:

  * g++>=4.8 or clang>=3.3 (clang is suggested)
  * C++ 4.8 or higher
  * boost
  * cmake
  * Qt (if you want to build the GUI)

# 2 Installation of the commandline interface
In this directory, type:
```sh
mkdir build
cd build
cmake .. # TODO: describe help about SCA_DEBUG, -Wall, -g
make # we suggest make -j<n>, where n is the number of CPU cores
```

If you get boost compile errors now, try to disable boost::graph:
```
cmake -DNO_BOOST_GRAPH=1 ..
make
```

# 3 Installation of the Qt GUI
The gui is built directly with the other tools, if you have Qt installed.

# 4 Debugging
The sca toosuite gives you debug output exactly if both of the following
conditions apply:

  1. you compiled with `-DSCA_DEBUG` (see (1) of this file)
  2. you set `SCA_DEBUG=1` (or any other integer) in your shell

For the second condition, you can do either

  * `SCA_DEBUG=1 ./path/program`
  * `export SCA_DEBUG=1; ./path/program`

Debug output is always sent to stdout, so it does not affect pipelining.

