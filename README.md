# sudoku-solver
**A human like implementation of a sudoku solver.**

This program is a Sudoku solver.

It provides three independant methods to solve sudoku grids of various sizes, from 4x4 (toy sudoku), to 25x25, 9x9 being the classical and usual size.

- The first and least efficient method is the _brut force_.
It tests all the possibilities in each cell and checks that each combination of filled grid is valid with regards to the sudoku rules (unicity in rows, columns and squares).
Filling the empty cells and then checking validity would lead to failure, as the number of operations to check all the combinations exceeds the capacity of any computer. The technique is to check validity after each cell is filled, shortening inspection of any invalid grid. 
This method can be invocated with option -B.

- The second method, the most efficient, uses an _exact cover search_ method. It implements the dancing links algorithm.
This method can be invocated with option -E.

- The third method, the most instructive, follows a _human logical process_ to determine the differents necessary steps in order to solve the grid.
This method is invocated by default.

**Human logical rules solver**

It proceeds with 4 logicals rules
    - cell exclusion
    - candidate exclusion
    - region exclusion
    - backtracking, only if the previous 3 rules have failed

An article (below) issued Feb 01, 2006 explains the two firsts rules in detail :
Sudoku & Graph Theory

By Eytan Suchard, Raviv Yatom, and Eitan Shapir,  Dr. Dobb's Journal
Feb 01, 2006
URL:http://www.ddj.com/cpp/184406436

Eytan, Raviv, and Eitan are software engineers in Israel. They can be contacted
at esuchard@012.net.il, ravivyatom@bezeqint .net, and eitans@ima.co.il,
respectively.

Sudoku is a logic puzzle in which there are 81 cells (vertices) filled with
numbers between 1 and 9. In each row, the numbers 1,2,3,..,9 must appear without
repetition. Likewise, the numbers 1,2,3,..,9 must appear without repetition in
the columns. In addition to the row and column constraints, the numbers
1,2,3,..,9 must appear in the nine nonoverlapping 3×3 subsquares without
repetition. So in short, the puzzle board is separated into nine blocks, with
nine cells in each block.

Two rules, "Chain Exclusion" and "Pile Exclusion", can be used to successfully
to fill in missing numbers for solving logical Sudoku puzzles (together with
region intersection analysis.)
Illogical Sudoku puzzles can also be solved, but require guesses.

We refer to possible numbers that should be assigned to a row, column, or one of
the nine 3×3 subsquares as a "Permutation Bipartite Graph" or nodes. A node
consists of a vector of n>1,n=2,3,4... vertices and all possible numbers that
can be assigned to these vertices, such that there exists at least one possible
match between the vertices of the vector and the numbers 1,2,...n.
For example, the following are nodes:

    ({1,2,3,5},{2,3},{2,3,4},{3,4},{4,5}, n=5
    ({1,2,3,7},{3,6},{3,4},{1,4},{5,6,7},{4,6},{2,7},
    {8,9},{8,9}, n=9

A possible match for the first vector is easy:

    1 -> {1,2,3,5}
    2 -> {2,3}
    3 -> {2,3,4}
    4 -> {3,4}
    5 -> {4,5}

A possible match for the second vector is more tricky:

    2 -> {1,2,3,7}
    3 -> {3,6}
    4 -> {3,4}
    1 -> {1,4}
    5 -> {5,6,7}
    6 -> {4,6}
    7 -> {2,7}
    8 -> {8,9}
    9 -> {8,9}

A number can be only assigned to a vertex that contains the possibility of
assigning that number. For instance, only the following possibilities are
accepted:

    7 -> {2,7} or 2 -> {2,7}.

Pile Exclusion and Chain Exclusion provide the basis of logical elimination
rules.

To understand Pile Exclusion, consider the following nodes:

    ({1,2,3,5},{3,6},{3,4},{5,6},{1,7,8,9},{4,6},{5,7,8,9},
    {4,6},{6,7,8,9},{1,4}, n=9

The numbers 7,8,9 appear only in three vertices:

    {1,7,8,9},{5,7,8,9},{6,7,8,9}

Because there is at least one possible match in the Permutation Bipartite Graph,
one vertex will be matched to 7, one to 8, and one to 9. Thus, you can erase the
other numbers from these three vertices to get the following three augmented
vertices:

    {1,7,8,9} -> {7,8,9}
    {5,7,8,9} -> {7,8,9}
    {6,7,8,9} -> {7,8,9}

and the entire Permutation Bipartite Graph becomes:

    ({1,2,3,5},{3,6},{3,4},{5,6},{7,8,9},{7,8,9},{4,6},
    {7,8,9},{1,4}), n=9

As for Chain Exclusion, consider these nodes:

    ({1,2,3,7},{3,6},{3,4},{1,4},{5,6,7},{4,6},{2,7},{8,9},
    {8,9}, n=9

In the second, third, and sixth positions in the vertices vector, you have:

    {3,6},{3,4},{4,6}

Only the numbers 3,4,6 can be assigned to these vertices. From this, you infer
that 3,4,6 are not a matching option in any of the remaining vertices. Thus, you
can erase these numbers from all the other vertices, resulting in a new, more
simple graph:

    ({1,2,7},{3,6},{3,4},{1},{5,7},{4,6},{2,7},{8,9},
    {8,9}, n=9

You can do the same thing with {1}, so that the resulting graph is:

    ({2,7},{3,6},{3,4},{1},{5,7},{4,6},{2,7},{8,9},
    {8,9}, n=9

Denis Berthier
Pattern-Based Constraint Satisfaction and Logic Puzzles (c) 2012
- For any valid Sudoku resolution rule, the rule deduced from it by permuting
  systematically the word "row" and "column" is valid.

- For any valid Sudoku resolution rule mentioning only numbers, rows and columns
  (i.e. neither blocks nor squares nor any property referring to such objects),
  any rule deduced from it by any systematic permutation of the words "number",
  "row" and "column" is valid.

- For any valid Sudoku resolution rule mentioning only numbers, rows and columns
  (i.e. neither blocks nor squares nor any property referring to such objects),
  if such rule displays a systematic symmetry between rows and columns but it can
  be proved without using the axiom on columns, then the rule deduced from it by
  systematically replacing the word "row" by "block" and the word "column" by
  "square" is valid.


**Builing executable**

Files for the sudoku solver library:

- solve.h: interface for the solver
- solve_mask.c: implementation of the solver

Files for the unit testing:

- terminal.h: defines the interface to communicate with the standard output terminal.
- terminal.c: implementation and declaration of the callback function for event and message handlers.
- main.c: calls the solver for the user defined grid. Use option `-h` for usage.
- Top95.sudoku: list of grids
- sudoku.ksh: a script that solves the grids declared in Top95.sudoku

The directive SUDOKU_SIZE can be set at compile time (using the compiler option -D SUDOKU_SIZE=*n*), between 2 and 5, to specify the size of grids the solver will solve.
Otherwise, SUDOKU_SIZE defaults to 3, for a 9x9 standard grid.

SUDOKU_SIZE actually defines the size of squares sides.
Therefore,

- 2 defines a sudoku grid of 4x4,
- 3 defines a sudoku grid of 9x9,
- 4 defines a sudoku grid of 16x16,
- and 5 defines a sudoku grid of 25x25.

Use the following command to build the executable:

    clang -DSUDOKU_SIZE=3 main.c terminal.c solve_mask.c ../knuth_dancing_links -ldlx

The Makefile can also be invocated by *make*.

The library dlx is build by the project [https://github.com/farhiongit/dancing-links](https://github.com/farhiongit/dancing-links).

Execute the program with the option -h for a user manual.

**References**

The implementation:

- Makes use of [dancing links library](https://github.com/farhiongit/dancing-links) for the implementation of the
Exact cover search method of resolution.
Environnement variable `LIBPATH` should be set to the directory containing the library `libdlx.a` before compiling wiht `make`.

- Makes use of [GNU `gettext`](https://www.gnu.org/software/gettext/manual/index.html) for the internationalization of messages displayed.
Files po/en_US.po and po/fr.po contains messages in english and in french.
