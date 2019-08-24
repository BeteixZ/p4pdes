best practices for C codes in `p4pdes/c/`
-----------------------------------------

My goal for myself is to use the following best practices.  I do not always
succeed:

  1. A feature without a regression test is broken.
  2. A code which is not valgrind-clean is broken.
  3. Any important code idea should have at least one associated exercise.
     * When the exercise is to build a code, the solution will be in `solns/`.
  4. The reader should see high-level declarations first, then `main()`, and
     then further details.
     * All non-static functions should be declared with a prototype (e.g.
       `extern int foo(double);`) just before `main()`.
     * The definitions follow `main()`.
     * `static` helper functions are an exception.
  5. When in doubt the style should follow the Style Guide in the
     [PETSc Developer's Guide](http://www.mcs.anl.gov/petsc/developers/developers.pdf).
     * A collection of examples is not a library API, so rigidly following
       this Guide would be foolish.
  6. If you do `./code -help intro` then you should see a description of the
     code and its option prefix `xxx_`.
  7. Do `./code -help |grep xxx_` to get documentation for code-specific options.

