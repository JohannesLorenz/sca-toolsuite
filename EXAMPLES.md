# Examples
This file lists useful examples for some tools. For more examples, please
consider the file `src/test.sh`.

First, some basic grid operations.

```sh
# use the coords script
core/create 8 8 | math/add `./coords 8 3 5` `./coords 8 5 3`

# manipulate grid via sequences or equations
core/create 8 8 1 | io/field_to_seq | sed 's/ [^ ]* / /g' | io/seq_to_field 8 8
core/create 9 9 0 | math/equation 'x=y||x=8-y'
core/create 20 20 0 | math/equation '(x-8)*(x-8)+(y-8)*(y-8)>16'

# moving a rectangle
core/create 10 10 | math/equation 'x<=4&&y<=4' | io/field_to_seq | math/calc "x+1+10" | io/seq_to_field 10 10

More topics using cellular automata are following.

```sh
# export to latex/TIKZ
echo -e "grids\n\n0\n\n`core/create 8 8 0|math/equation 8*x+y`\n\npath\n\n0\n\n\nrgb32\n\nv:=((v&7)<<5)|((v&(7<<3))<<10)\n\n\n" | \
io/tik complete > tmp/tmp.tex

# run game of life for 3 steps
core/create 9 6 0 | math/add 10 11 19 20 32 33 34 | ca/ca "`cat ../data/ca/game_of_life.txt`" 3

# transform image, using a CA on ARGB pixels
img/transform "v&(255)|(255<<24)" < in.png > out.png
```

The following operations are from the `algo` directory. They are for sandpile
CA only.

```sh
# random throw
core/create 1000 1000 | algo/random_throw random 2150000 42 > end_configuration.txt
core/create 8 8 | algo/random_throw random 128 42 l | io/avalanches_bin2human 8 ids

# show 1D avalanche graphically
core/create 9 9 3 | math/add `./coords 9 4 4` | algo/fix l `./coords 9 4 4` | io/avalanches_bin2human 9 | io/seq_to_field 9 9

# using a filter to show which cells decreased
core/create 7 7 3 | algo/throw `./coords 7 3 3` | math/equation "v<=2"
# use the fix command for grids with multiple fields with 4 or more grains
core/create 8 8 8888 | algo/fix s

# burning test
core/create 8 8 1 | algo/is_recurrent
core/create 8 8 2 | algo/burning_test | io/avalanches_bin2human 8  | io/seq_to_field 8 8
```