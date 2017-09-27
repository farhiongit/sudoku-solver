
/**
 * @file
 * Invokes sudoku solver.
 */

/***********************************************************************************
* Author: Laurent Farhi                                                            *
* Name: main.c                                                                     *
* Language: C                                                                      *
* Copyright (C) 2009, All rights reserved.                                         *
*                                                                                  *
* RETURN CODE:                                                                     *
* Non zero on success (the grid has at least one solution), zero otherwise.        *
*                                                                                  *
* LICENSE:                                                                         *
* This program is free software; you can redistribute it and/or modify             *
* it under the terms of the GNU General Public License as published by             *
* the Free Software Foundation; either version 2 of the License, or                *
* (at your option) any later version.                                              *
*                                                                                  *
* This program is distributed in the hope that it will be useful,                  *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
* GNU General Public License for more details.                                     *
*                                                                                  *
* You should have received a copy of the GNU General Public License                *
* along with this program; if not, write to the Free Software                      *
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA       *
***********************************************************************************/

#define _XOPEN_SOURCE
#define _BSD_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <ctype.h>
#include <locale.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "solve.h"
#include "terminal.h"

/// Called on exit.
static void
bye (void)
{
  terminal_unset ();

  printf ("That was all, folks.\n");
}

/// Read the next digit from string
/// @param[in] initString string to be read
/// @return read character or \c EOF if no more digits found
///
/// Can be called repeatedly with the same input string to return successively each digit. 
static int
getdigit (const char *initString)
{
  static const char *pos = 0;
  static const char *arg = 0;

  if (arg != initString)        // argument has changed since last call
  {
    pos = initString;
    arg = initString;
  }

  int c = 0;

  do
  {
    if (pos && (c = *(pos++)))  // char to int conversion here
      /* nothing */ ;
    else if (!initString && (c = getchar ()) != EOF)
      /* nothing */ ;
    else
    {
      c = EOF;
      pos = 0;                  // stops processing if getdigit is still called when end of initString has been reached already.
    }

    if (c == '.')
      c = sudoku_grid_referential.empty_code;

    char *pc;

    if ((pc = strchr (sudoku_grid_referential.value_name, toupper (c))) ||
        (pc = strchr (sudoku_grid_referential.value_name, tolower (c))))
      return (*pc);
    else if (tolower (c) == tolower (sudoku_grid_referential.empty_code))
      return (sudoku_grid_referential.empty_code);
    else if (c == EOF)
      return (c);
  }
  while (1);

  fprintf (stderr, "Unexpected error (%s, %s, %i).\n", __func__, __FILE__, __LINE__);
  exit (-1);
}

/// Well, that's the entry point.
int
main (int argc, char *argv[])
{
  sudoku_init ();

  const char *TEST_GRID[] = {
#if SUDOKU_SIZE == 3
    "8........" "..36....." ".7..9.2.." ".5...7..." "....457.." "...1...3." "..1....68" "..85...1." ".9....4..",
    "000000010" "400000000" "020000000" "000050604" "008000300" "001090000" "300400200" "050100000" "000807000",
    "400009000" "030010020" "006700000" "001000004" "050200070" "800000600" "000004008" "070030010" "000500900",
    "2...84..." ".93......" ".819...73" "......2.." ".3.8....5" "71..5...." "9.7......" "....3.6.7" "..8.46...",
    "5.......9" ".2.1...7." "..8...3.." ".4...2..." "....5...." "...7.6.1." "..3...8.." ".6...4.2." "9.......5",
    "1.......2" ".9.4...5." "..6...7.." ".5.9.3..." "....7...." "...85..4." "7.....6.." ".3...9.8." "..2.....1",
    "7...85..." ".81......" ".43....59" "......3.1" "2..4..7.." ".3...7.9." ".15......" "....5.2.3" "....98...",
    "7...85..7" ".81......" ".43....59" "......3.1" "2..4..7.." ".3...7.9." ".15......" "....5.2.3" "....98...",
    "76.5..2.." "1.2.4..78" "..4..851." ".....3..." "..71.2..." "9...876.." "..6....3." ".1.7..8.." ".43..9...",        // BestWestern Ypsilon Essen (fake sudoku)
#elif SUDOKU_SIZE == 2
    "1234 4.2. .4.. 2..3",
#elif SUDOKU_SIZE == 4
    "..f65ge7b.1..83..b..d.6.......ef8.......95.e4..7e..1.....f2..6..g6....da..fb.c..f....4951..28..e24..713.....9g..3...e6.g..a..d.4a.8..5..f.3c...6..4b.....ad6..8g6..53..e298....1..9.6d..e4....c2..a..e1.....3..84..39.8d.......5d8.......e.7..f..9g..a.2584361..",
#else
    "",
#endif
  };

  setlocale (LC_ALL, "");

  display display = 0;
  method method = ELIMINATION;
  findSolutions find = ALL;
  int test = -1;
  int iflag = 0;
  int quiet = 0;

  // Command-line options
  const char options[] = "qivgrchfBET:";

  opterr = 1;
  for (int letter = 0; (letter = getopt (argc, argv, options)) >= 0;)
  {
    if (letter == '?')
    {
      printf ("Type '%s -h' for help.\n", argv[0]);
      exit (0);
    }
    else if (letter == 'h')
    {
      printf ("Name:\n  %s\n", basename (argv[0]));
      printf ("\nDescription:\n  Sudoku Solver using logical rules for elimination of candidates.\n");
      printf ("\nVersion:\n  %s\n", sudoku_get_version ());
      printf ("\nUsage:\n  %s [-vh] [-fBE] [-igcrq] [-T n] [grid]\n", basename (argv[0]));
      printf ("\nArgument:\n");
      printf ("    'grid' is the sequence of the %1$i characters (%2$ix%3$i cells) of the sudoku grid :\n",
              GRID_SIZE * GRID_SIZE, GRID_SIZE, GRID_SIZE);
      printf
        ("      %1$s, %2$c or . for an empty cell, other characters (including space and end-of-line) are ignored.\n",
         sudoku_grid_referential.value_name, sudoku_grid_referential.empty_code);
      printf ("      For example:\n\n");
      if (strlen (TEST_GRID[0]))
        printf ("        %s\n", TEST_GRID[0]);
#if SUDOKU_SIZE == 3
      if (strlen (TEST_GRID[0]))
        printf ("\n      or");
      printf ("\n\n        7...85....81.......43....59......3.12..4..7...3...7.9..15..........5.2.3....98...\n");
#endif
      printf ("\n    If the argument 'grid' is omitted or is '-', it is read from the standard input.\n");
      printf ("\nOptions:\n");
      printf ("  General options:\n");
      printf ("   -v\tDisplay version\n");
      printf ("   -h\tDisplay this help page\n");
      printf ("\n");
      printf ("  Solving options:\n");
      printf ("   -f\tSearch for the first solution only rather than all of them\n");
      printf ("\n");
      printf ("  Default method is elimination method (human like, using logical rules.)\n"
              "  Other methods are optionnally available :\n");
      printf ("   -B\tSolve using backtracking method (brute force)\n");
      printf ("   -E\tSolve using exact cover search method (dancing links)\n");
      printf ("\n");
      printf ("  Display options for elimination method only:\n");
      printf ("   -i\tInteractive mode (step by step)\n");
      printf ("   -g\tDisplay grid while processing\n");
      printf ("   -c\tDisplay grid with candidates while processing\n");
      printf ("   -r\tDisplay logical rules\n");
      printf ("   -q\tCompletely quiet\n");
      printf ("\n");
      printf ("  Options for test purpose:\n");
      printf ("   -T n\tSolve test grid number n, n between 1 and %lu (for test purpose)\n",
              sizeof (TEST_GRID) / sizeof (const char *));
      printf ("\n");
      printf ("Return value:\n"
              "    0\tNo solution were found.\n"
              "    1\tA solution was found, without using backtracking.\n"
              "    2\tA solution was found, using backtracking.\n");

      exit (0);
    }
    else if (letter == 'v')
    {
      printf ("Version (-v) : %s, Sudoku Solver %s\n", basename (argv[0]), sudoku_get_version ());
      exit (0);
    }
    else if (letter == 'i')
      iflag = 1;
    else if (letter == 'f')
      find = FIRST;
    else if (letter == 'g')
      display = display | NORMAL | RULES;
    else if (letter == 'r')
      display = display | RULES;
    else if (letter == 'c')
      display = display | VERBOSE | RULES;
    else if (letter == 'B')
      method = BACKTRACKING;
    else if (letter == 'E')
      method = EXACT_COVER;
    else if (letter == 'q')
      quiet = 1;
    else if (letter == 'T')
    {
      char *endptr = 0;

      if ((test = strtol (optarg, &endptr, 10)) < 0 || *endptr)
        test = 0;;
    }
  }

  if (!quiet)
  {
    printf ("Method : %s.\n",
            (method == EXACT_COVER ? "exact cover search (-E)" : method ==
             BACKTRACKING ? "backtracking (-B)" : "elimination of candidates"));
    if (find == ALL)
      printf ("Searching all solutions.\n");
    else
      printf ("Searching first solution only (-f).\n");
  }

  // Grid initialization
  int initGrid[GRID_SIZE][GRID_SIZE];

  const char *initString = 0;

  if (test >= 0)                // for demo or debugging purposes
  {
    if (test >= 1 && test <= sizeof (TEST_GRID) / sizeof (const char *))
    {
      if (!quiet)
        printf ("Solving test grid #%i (-T%i, command line arguments ignored).\n", test, test);
      initString = TEST_GRID[test - 1];
    }
    else
    {
      fprintf (stderr, "Invalid option argument for option -T: valid values betweeen 1 and %lu.\n",
               sizeof (TEST_GRID) / sizeof (const char *));
      exit (-1);
    }
  }
  else if (argc > optind && strcmp (argv[optind], "-"))
    initString = argv[optind];  // Command-line argument

  if (!initString)
  {
    printf
      ("Type in the %1$i cells (%2$s, %3$c or . for an empty cell, other characters, including space and end-of-line, ignored) and end with Control-D.\n",
       GRID_SIZE * GRID_SIZE, sudoku_grid_referential.value_name, sudoku_grid_referential.empty_code);
    printf ("(Use option -h for usage information.)\n");
  }

  int i = 0, c;

  for (i = 0; i < GRID_SIZE * GRID_SIZE && ((c = getdigit (initString)) != EOF); i++)
    initGrid[i / GRID_SIZE][i % GRID_SIZE] =
      (c ==
       sudoku_grid_referential.empty_code ? 0 : (strchr (sudoku_grid_referential.value_name, c) -
                                                 sudoku_grid_referential.value_name + 1));

  if (i < GRID_SIZE * GRID_SIZE)
  {
    fprintf (stderr, "Incomplete grid (%1$i values provided for initialization, %2$i values needed.)\n", i,
             GRID_SIZE * GRID_SIZE);
    exit (-1);
  }

  for (i = 0; getdigit (initString) != EOF; i++)
    /* exhaust input stream */ ;
  if (i > 0)
    fprintf (stderr, "Warning: %i values ignored from input stream.\n", i);

  // Terminal standard settings
  if (!quiet)
  {
    display = terminal_display_set (display);

    if (atexit (bye) != 0)
    {
      fprintf (stderr, "Cannot set exit function\n");
      exit (EXIT_FAILURE);
    }
    terminal_set (iflag);

    printf ("Grid is: ");
    for (i = 0; i < GRID_SIZE * GRID_SIZE; i++)
      printf ("%c",
              initGrid[i / GRID_SIZE][i %
                                      GRID_SIZE] ? sudoku_grid_referential.value_name[initGrid[i / GRID_SIZE][i %
                                                                                                              GRID_SIZE]
                                                                                      - 1] : '.');
    printf ("\n");
  }

  // Processing...
  int ret = sudoku_solve (initGrid, method, find);

  exit (ret == EXACT_COVER ? 3 : ret == BACKTRACKING ? 2 : ret == ELIMINATION ? 1 : 0);
}                               // main
