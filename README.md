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

# Examples
There are lots of examples in the file `src/test.sh`. A small selection:

```sh
# use the coords script
core/create 8 8 | math/add `./coords 8 3 5` `./coords 8 5 3`

# manipulate grid via sequences or equations
core/create 8 8 1 | io/field_to_seq | sed 's/ [^ ]* / /g' | io/seq_to_field 8 8
core/create 9 9 0 | math/equation 'x=y||x=8-y'
core/create 20 20 0 | math/equation '(x-8)*(x-8)+(y-8)*(y-8)>16'

# moving a rectangle
core/create 10 10 | math/equation 'x<=4&&y<=4' | io/field_to_seq | math/calc "x+1+10" | io/seq_to_field 10 10

# show 1D avalanche graphically
core/create 9 9 3 | math/add `./coords 9 4 4` | algo/fix l `./coords 9 4 4` | io/avalanches_bin2human 9 | io/seq_to_field 9 9

# using a filter to show which cells decreased
core/create 7 7 3 | algo/throw `./coords 7 3 3` | math/equation "v<=2"

# export to ODS format (OpenOffice)
core/create 4 4 4 | io/to_ods four

# random throw
core/create 1000 1000 | algo/random_throw random 2150000 42 > end_configuration.txt
core/create 8 8 | algo/random_throw random 128 42 l | io/avalanches_bin2human 8 ids

# use the fix command for grids with multiple fields with 4 or more grains
core/create 8 8 8888 | algo/fix s

# burning test
core/create 8 8 1 | algo/is_recurrent
core/create 8 8 2 | algo/burning_test | io/avalanches_bin2human 8  | io/seq_to_field 8 8

# run game of life for 3 steps
core/create 9 6 0 | math/add 10 11 19 20 32 33 34 | ca/ca "`cat ../data/ca/game_of_life.txt`" 3

# transform image, using a CA on ARGB pixels
img/transform "v&(255)|(255<<24)" < in.png > out.png
```

# 3 Thanks to
In alphabetical order, the author thanks:
  * All people who give support for bost::spirit [6].
  * Oliver Schneider. (TODO: Website?)
  * Philipp Lorenz.
  * Sebastian Frehmel, his diploma thesis at [4].
  * Dr Thomas Worsch [2] @ The Karlsruhe Institute of Technology (KIT).

# 3 Links
[2] http://liinwww.ira.uka.de/~thw/
[4] http://www.sebastianfrehmel.de/studium/diplomarbeit/
[6] http://boost-spirit.com/home/feedback-and-support/

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
  * [INSTALL](INSTALL.txt)
  * [LICENSE](LICENSE.txt)

