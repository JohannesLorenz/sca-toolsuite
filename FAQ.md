# Introduction
This file contains questions often asked by others.
Or questions I think others might ask.

**Contents**

  1. General
  2. Compiling
  3. Sandpiles

# 1 General
```
Q: Are there any other COPYING restrictions than GPL3?
A: None.

Q: I miss the ternary operator in the equation solver...
A: We have added it now. If you still miss it, please update your local
   version of the toolsuite.

Q: I need `--help` as a filename...
A: Try to pass `./--help` to the program.

Q: Is there a project file for my code editor?
A: If you are using qt creator: Yes. Use src/sca-toolsuite.pro.

Q: Is Qt necessary?
A: No, the .pro file is only for people who use qt creator for coding. The
   Makefile generation is done by cmake.
```

# 2 Compiling
```
Q: My compiler swaps a lot while compiling
A: This is mostly a problem of g++. You can try clang. Or, you should always
   compile with `make -j 1`.

Q: I get a lot of warnings about unused local typedefs or maybe uninitialized
   variables. Why?
A: If this happens, your boost libraries are too old for your compiler. You'll
   find a line in our CMakeLists (`WARN_EXC`) which you can enable to ignore
   the errors.

Q: libres is compiled once for multiple binaries. What if I need different
   versions of it for different executables in `./src`?
A: We think that this happens seldom. If it does, you could either make two
   different builds of sca-toolsuite (suggested), or hack into the CMakeLists.
```

# 3 Sandpiles
```
Q: How fast is the sandpile algorithm?
A: There are different experiments:

     * stabilize: stabilize one given (positive) configuration.
     * random throw: throw one grain at random position, then stabilize.
       Repeat it often enough. Runtime counted is started when a recurrent
       configuration is reached.

   For stabilize, we are faster if the grid contains a bit less than 3 grains
   on each cell. For random throw, we are much faster.

Q: How fast is the sandpile algorithm for the random throw?
A: We compared some algorithms in BENCHMARKS.txt.

Q: What if we want to log avalanches in random throw?
A: Then we are even a lot faster, since n^2 avalanches can be thrown at once.

Q: Why is the sandpile algorithm so fast, even though it is sequential?
A: The reason for the speedup is that avalanches are developped in multiple
   levels, where in each level, each cell on the grid is assured to only fire
   once. Thus, we don't need to assure that a cell is not added multiple times
   to the stack of active cells. This makes branch prediction and memory usage
   much easier.

Q: Can our sequential sandpile algorithm be parallelized?
A: Maybe. We have not tested it yet.

Q: Can our sequential sandpile algorithm be improved otherwise?
A: We assume so. However, the current algorithm is already very short. If
   there are any improvements, then probably not the algorithm itself, but
   maybe changes in memory alignment or completely different algorithms.
```
