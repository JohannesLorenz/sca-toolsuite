# Knowledge

While developing this project, a lot of knowledge was gained about algorithms:

* Improving the ASM algorithm can be very difficult, only minor changes may
  not only make it invalid, but can also make it a lot slower, even when you
  would not expect this:
  - Replacing additions by bitwise or (and similar things): 0% difference
  - Using link time optimization: 5-10% slower
  - Using unsigned int instead of signed: 5% slower
  - Refactoring code that should not have influence (in this case, making a
    common function in the ASM algorithm to handle north, east, south and
    west): 5-10% slower
  - Using any memory lookups, like hashing etc, is *very* expensive. For this
    algorithm, computations are often faster than memory lookup.
