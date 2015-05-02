# Introduction

This file should help you getting the usage of all programs.

**Contents**

  1. Commandline Help
  2. Coordinates
  3. Input and Output Types
  4. Doxygen

# 1 General Help

For every non-shell program, you can use `--help` to get a full help.
(Note: If you need a file name for argv[1] which is `--help`, write
`./--help`). For shell programs, look into the files and find out which
programs they call.

For syntax in the help descriptions, the following rules are used:

  * `l|s` means that either `l` or `s` is allowed (not both).
  * `[argument]` means that `argument` can be omitted.
  * `<argument-description>` means that you should not see `argument-description` as a
    string, but interpret it: e.g. call add `<number>` as `add 1` and not as `add number`.

# 2 Coordinates

For user interaction, cells are always numbered equally. Borders are not counted.
Numeration is

  * beginning with zero in the top left corner
  * increasing first by rows, then by columns

For using xy-coordinates, there is the `coords` script. Try `./coords --help`.

For programmers there are two formats to know:

  * The *human* format, excluding borders, just described as above
  * The *internal* format, like the human format, but including borders.
    It is used for all internal computations.

# 3 Input and Output types

There are three types:

  * Sequences of numbers are ASCII printed numbers, separated by spaces or newlines
    (more exactly, you can use everything as a separator that fscanf sees as such)
  * Grids are ascii printed numbers in a rectangle, i.e. multiple rows of equal length
    of number sequences
  * The binary avalanche output format is not human readable for efficiency reasons.
    You can convert it to an ASCII sequence using `io/avalanches_bin2human`

# 4 Doxygen

Switch to the `doc` directory and type
```sh
./generate.sh`
```
This will generate doxygen help.

sca-toolsuite has Doxygen help. To generate it, go to your build and type
```
make doc
```
This will create multiple folders. For example, you can view
doc/html/index.html to start browsing the html documentation.

