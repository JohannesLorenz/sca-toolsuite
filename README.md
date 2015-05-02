# Greetings
Welcome! Thank you for considering the README file.

**Contents**

  1. What is the sca-toolsuite?
  2. Why using the sca-toolsuite
  3. Thanks to
  4. Links
  5. Contact
  6. Enclosed files

# 1 What is the sca-toolsuite?

The name *sca-toolsuite* stands for **sca cellular automata toolsuite**. Most
of its tools can operate on every cellular automaton (CA) which is

  1. **synchronous** or **purely asynchronous** and
  2. uses a **finite, two-dimensional grid**

This toolsuite provides you with a number of tools to

  * create, read or write grids
  * exporting them to *TGA*, *latex* and *ODS* format
  * do math with them like in C++
  * do simulations
  * do very fast routines, especially for sandpile CA

Most of the tools can be combined using UNIX pipes (see examples in point 5).

# 2 Why using the sca-toolsuite?
Reasons for using it include the following.

  * The sca-toolsuite can simulate **CA of many different types** with both
    high **flexibility** and high **efficiency**. One example of flexibility
    is the piping mechanism:
    ```sh
    core/create 20 20 4 |
    ca/ca 'v+(-4*(v>=4))+(a[-1,0]>=4)+(a[0,-1]>=4)+(a[1,0]>=4)+(a[0,1]>=4)'
    ```
  * **For sandpiles, we provide the (probably) worlds fastest algorithm**. Our
    computation time was 37 per cent of the fastest algorithm we knew of
    before. For details, see `6 Enclosed files`.
  * A **GUI**. It works for all CA.
  * **Bug safety** is important: Algorithms are kept simple and are being
    tested automatically.
  * A large **documentation**: doxygen and --help for all C++ programs.
  * Besides cellular automata, there are many more applications of this
    toolsuite:
    * **rotor-routing** (as described by Priezzhev et al, 1996)
    * **image manipulation**, e.g. with filters
    * **calculating maths**
    * running two-dimensional games easily (in progress)
    * spreadsheet (future feature)
    * ...

# 3 Thanks to
In alphabetical order, the author thanks:
  * All people who give support for bost::spirit [6].
  * Oliver Schneider. (TODO: Website?)
  * Philipp Lorenz.
  * Sebastian Frehmel, his diploma thesis at [4].
  * Dr Thomas Worsch [2] @ The Karlsruhe Institute of Technology (KIT).

# 3 Links
Links from this document:
  2. http://liinwww.ira.uka.de/~thw/
  4. http://www.sebastianfrehmel.de/studium/diplomarbeit/
  6. http://boost-spirit.com/home/feedback-and-support/

# 4 Contact
Feel free to give feedback. My e-mail address is shown if you execute this in
a shell:
```sh
printf "\x6a\x6f\x68\x61\x6e\x6e\x65\x73\x40\x6c\x6f\
\x72\x65\x6e\x7a\x2d\x70\x72\x69\x76\x61\x74\x2e\x6e\x65\x74\x0a"
```

# 6 Enclosed files
The following files provide more information:
  * [BENCHMARKS](BENCHMARKS.md)
  * [DOCUMENTATION](DOCUMENTATION.md)
  * [EXAMPLES](EXAMPLES.md)
  * [FAQ](FAQ.md)
  * [INSTALL](INSTALL.md)
  * [LICENSE](LICENSE.txt)

