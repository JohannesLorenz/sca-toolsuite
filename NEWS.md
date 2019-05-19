# News

* 2012-05-27: Initial version of new ASM algorithm that never lets the
  starting cell fire twice. This results in no cell firing twice, thus in a
  stack of predictable size and less if statements with possible branch
  mispredictions. The algorithm is faster than the currently best known
  algorithm (algorithm by diploma thesis, Sebastian Frehmel, KIT,
  implemented by bachelor thesis, Johannes Lorenz, KIT). More exactly:
  73% faster with 1 instead of 8 threads on an Intel Core i5 from 2013.
* 2014-04-26: Improvements (20-30%) for ASM by using raw pointers instead of
  indices for the stack (see [benchmarks](BENCHMARKS.md))
* 2019-06-22: Improvements (5%) for ASM by elliminating all critical ifs to
  avoid branch mispredictions

