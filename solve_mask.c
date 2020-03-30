
/**
 * @file
 * Implements algorithms to solve sudoku puzzles.
 *
 * This program is a Sudoku solver.
 * It proceeds with 4 logicals rules
 *     - cell exclusion
 *     - candidate exclusion
 *     - region exclusion
 *     - backtracking, only if the previous 3 rules have failed
 *
 * An article (below) issued Feb 01, 2006 explains the two firsts rules in detail :
 * Sudoku & Graph Theory
 *
 * By Eytan Suchard, Raviv Yatom, and Eitan Shapir,  Dr. Dobb's Journal
 * Feb 01, 2006
 * URL:http://www.ddj.com/cpp/184406436
 *
 * Eytan, Raviv, and Eitan are software engineers in Israel. They can be contacted
 * at esuchard@012.net.il, ravivyatom@bezeqint .net, and eitans@ima.co.il,
 * respectively.
 *
 * Sudoku is a logic puzzle in which there are 81 cells (vertices) filled with
 * numbers between 1 and 9. In each row, the numbers 1,2,3,..,9 must appear without
 * repetition. Likewise, the numbers 1,2,3,..,9 must appear without repetition in
 * the columns. In addition to the row and column constraints, the numbers
 * 1,2,3,..,9 must appear in the nine nonoverlapping 3×3 subsquares without
 * repetition. So in short, the puzzle board is separated into nine blocks, with
 * nine cells in each block.
 *
 * Two rules, "Chain Exclusion" and "Pile Exclusion", can be used to successfully
 * to fill in missing numbers for solving logical Sudoku puzzles (together with
 * region intersection analysis.)
 * Illogical Sudoku puzzles can also be solved, but require guesses.
 *
 * We refer to possible numbers that should be assigned to a row, column, or one of
 * the nine 3×3 subsquares as a "Permutation Bipartite Graph" or nodes. A node
 * consists of a vector of n>1,n=2,3,4... vertices and all possible numbers that
 * can be assigned to these vertices, such that there exists at least one possible
 * match between the vertices of the vector and the numbers 1,2,...n.
 * For example, the following are nodes:
 *
 *     ({1,2,3,5},{2,3},{2,3,4},{3,4},{4,5}, n=5
 *     ({1,2,3,7},{3,6},{3,4},{1,4},{5,6,7},{4,6},{2,7},
 *     {8,9},{8,9}, n=9
 *
 * A possible match for the first vector is easy:
 *
 *     1 -> {1,2,3,5}
 *     2 -> {2,3}
 *     3 -> {2,3,4}
 *     4 -> {3,4}
 *     5 -> {4,5}
 *
 * A possible match for the second vector is more tricky:
 *
 *     2 -> {1,2,3,7}
 *     3 -> {3,6}
 *     4 -> {3,4}
 *     1 -> {1,4}
 *     5 -> {5,6,7}
 *     6 -> {4,6}
 *     7 -> {2,7}
 *     8 -> {8,9}
 *     9 -> {8,9}
 *
 * A number can be only assigned to a vertex that contains the possibility of
 * assigning that number. For instance, only the following possibilities are
 * accepted:
 *
 *     7 -> {2,7} or 2 -> {2,7}.
 *
 * Pile Exclusion and Chain Exclusion provide the basis of logical elimination
 * rules.
 *
 * To understand Pile Exclusion, consider the following nodes:
 *
 *     ({1,2,3,5},{3,6},{3,4},{5,6},{1,7,8,9},{4,6},{5,7,8,9},
 *     {4,6},{6,7,8,9},{1,4}, n=9
 *
 * The numbers 7,8,9 appear only in three vertices:
 *
 *     {1,7,8,9},{5,7,8,9},{6,7,8,9}
 *
 * Because there is at least one possible match in the Permutation Bipartite Graph,
 * one vertex will be matched to 7, one to 8, and one to 9. Thus, you can erase the
 * other numbers from these three vertices to get the following three augmented
 * vertices:
 *
 *     {1,7,8,9} -> {7,8,9}
 *     {5,7,8,9} -> {7,8,9}
 *     {6,7,8,9} -> {7,8,9}
 *
 * and the entire Permutation Bipartite Graph becomes:
 *
 *     ({1,2,3,5},{3,6},{3,4},{5,6},{7,8,9},{7,8,9},{4,6},
 *     {7,8,9},{1,4}), n=9
 *
 * As for Chain Exclusion, consider these nodes:
 *
 *     ({1,2,3,7},{3,6},{3,4},{1,4},{5,6,7},{4,6},{2,7},{8,9},
 *     {8,9}, n=9
 *
 * In the second, third, and sixth positions in the vertices vector, you have:
 *
 *     {3,6},{3,4},{4,6}
 *
 * Only the numbers 3,4,6 can be assigned to these vertices. From this, you infer
 * that 3,4,6 are not a matching option in any of the remaining vertices. Thus, you
 * can erase these numbers from all the other vertices, resulting in a new, more
 * simple graph:
 *
 *     ({1,2,7},{3,6},{3,4},{1},{5,7},{4,6},{2,7},{8,9},
 *     {8,9}, n=9
 *
 * You can do the same thing with {1}, so that the resulting graph is:
 *
 *     ({2,7},{3,6},{3,4},{1},{5,7},{4,6},{2,7},{8,9},
 *     {8,9}, n=9
 *
 * Denis Berthier
 * Pattern-Based Constraint Satisfaction and Logic Puzzles (c) 2012
 * - For any valid Sudoku resolution rule, the rule deduced from it by permuting
 *   systematically the word "row" and "column" is valid.
 *
 * - For any valid Sudoku resolution rule mentioning only numbers, rows and columns
 *   (i.e. neither blocks nor squares nor any property referring to such objects),
 *   any rule deduced from it by any systematic permutation of the words "number",
 *   "row" and "column" is valid.
 *
 * - For any valid Sudoku resolution rule mentioning only numbers, rows and columns
 *   (i.e. neither blocks nor squares nor any property referring to such objects),
 *   if such rule displays a systematic symmetry between rows and columns but it can
 *   be proved without using the axiom on columns, then the rule deduced from it by
 *   systematically replacing the word "row" by "block" and the word "column" by
 *   "square" is valid.
 */
#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

/***********************************************************************************
* Author: Laurent Farhi                                                            *
* Name: solve.c                                                                    *
* Language: C                                                                      *
* Copyright (C) 2009, All rights reserved.                                         *
*                                                                                  *
* CONTACT:                                                                         *
* Email: lfarhi@sfr.fr                                                             *
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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "solve.h"
#include "../knuth_dancing_links/dancing_links.h"
#include "finally.h"
#include <libintl.h>
#include <assert.h>

#ifdef __USE_GNU_GETTEXT
// Internationalization
// The library code doesn’t call setlocale (LC_ALL, ""). It’s the responsibility of the main program to set the locale.
#define PACKAGE "SUDOKU_SOLVER"
#define _(String) dgettext (PACKAGE, String)
#else
#define _(String) String
#define bindtextdomain(PACKAGE, String)
#endif

/// Version of the implementation.
#define SUDOKU_SOLVE_VERSION "2.1"

/// Maximum length of messages to be displayed.
#define SUDOKU_MAX_MESSAGE_LENGTH 10001

/// Append text to the end of a string.
/// @param [in] str String to be appended.
/// @param [in] size Maximum length of the string.
/// @param [in] ... Variable list of arguments following the printf convention.
static int
strnadd (char *str, size_t size, ...)
{
  int ret;
  va_list ap;

  va_start (ap, size);
  const char *format = va_arg (ap, const char *);

  ret = vsnprintf (str + strlen (str), size - 1 - strlen (str), format, ap);
  va_end (ap);
  return ret;
}

#define MESSAGE_APPEND(rule, ...) \
strnadd (rule, sizeof(rule) / sizeof(*rule), __VA_ARGS__)
//snprintf (rule + strlen (rule), sizeof(rule) / sizeof(*rule) - 1 - strlen (rule), __VA_ARGS__)

//////// Event handler /////////
// Grid changed event
/// Definition of a list event handlers.
typedef struct sudoku_grid_event_handler_list
{
  sudoku_grid_event_handler handler;    ///< Function pointer
  struct sudoku_grid_event_handler_list *next;  ///< Pointer to the next element of the list
} sudoku_grid_event_handler_list;

/// Handlers to be called on grid initialization.
static sudoku_grid_event_handler_list *sudokuOnInitEventHandlers = 0;

/// Handlers to be called on grid change.
static sudoku_grid_event_handler_list *sudokuOnChangeEventHandlers = 0;

/// Handlers to be called on grid solved.
static sudoku_grid_event_handler_list *sudokuOnSolvedEventHandlers = 0;

void
sudoku_grid_event_handler_add (sudokuGridEventType type, sudoku_grid_event_handler handler)
{
  sudokuGridEventType t[3] = { ON_INIT, ON_CHANGE, ON_SOLVED };
  sudoku_grid_event_handler_list *const hls[3] =
    { sudokuOnInitEventHandlers, sudokuOnChangeEventHandlers, sudokuOnSolvedEventHandlers };
  for (int i = 0; i < 3; i++)
  {
    for (sudoku_grid_event_handler_list * ptr = hls[i]; ptr; ptr = ptr->next)
      if (ptr->handler == handler)      // handler already registered
        t[i] = 0;

    if (type & t[i])
    {
      sudoku_grid_event_handler_list *const pev = malloc (sizeof (*pev));

      if (pev == 0)
      {
        fprintf (stderr, _("Memory allocation error (%s, %s, %i)\n"), __func__, __FILE__, __LINE__);
        exit (-1);
      }
      pev->handler = handler;
      pev->next = 0;

      if (hls[i] == 0)
      {
        switch (t[i])
        {
          case ON_INIT:
            sudokuOnInitEventHandlers = pev;
            break;
          case ON_CHANGE:
            sudokuOnChangeEventHandlers = pev;
            break;
          case ON_SOLVED:
            sudokuOnSolvedEventHandlers = pev;
            break;
        }
      }
      else
      {
        sudoku_grid_event_handler_list *ptr;

        for (ptr = hls[i]; ptr->next != 0; ptr = ptr->next)
          if (ptr->handler == handler || ptr->next->handler == handler) // handler already registered
          {
            free (pev);
            return;
          }
        ptr->next = pev;
      }
    }
  }
}

/// Remove handler from the lists of handlers.
/// @param [in] type Types (or'ed) of handler to remove.
/// @param [in] handler Handler to be removed. 0 will remove all handlers.
void
sudoku_grid_event_handler_remove (sudokuGridEventType type, sudoku_grid_event_handler handler)
{
  sudokuGridEventType t[3] = { ON_INIT, ON_CHANGE, ON_SOLVED };
  sudoku_grid_event_handler_list *hls[3] =
    { sudokuOnInitEventHandlers, sudokuOnChangeEventHandlers, sudokuOnSolvedEventHandlers };

  sudoku_grid_event_handler_list *ptr, *next;

  for (int i = 0; i < 3; i++)
  {
    if (type & t[i])
    {
      if (!hls[i])
        return;
      while (hls[i] && (!handler || hls[i]->handler == handler))
      {
        ptr = hls[i]->next;
        free (hls[i]);
        hls[i] = ptr;
        switch (t[i])
        {
          case ON_INIT:
            sudokuOnInitEventHandlers = ptr;
            break;
          case ON_CHANGE:
            sudokuOnChangeEventHandlers = ptr;
            break;
          case ON_SOLVED:
            sudokuOnSolvedEventHandlers = ptr;
            break;
        }
      }
      if (!hls[i])
        return;
      for (ptr = hls[i]; ptr && ptr->next;)
      {
        if (ptr->next->handler == handler)
        {
          next = ptr->next->next;
          free (ptr->next);
          ptr->next = next;
        }
        else
          ptr = ptr->next;
      }
    }
  }
}

/// Call handler of type #ON_INIT.
/// @param [in] id Game identifier.
/// @param [in] evt_args Event arguments.
static void
sudoku_on_init (uintptr_t id, sudoku_grid_event_args evt_args)
{
  for (sudoku_grid_event_handler_list * ptr = sudokuOnInitEventHandlers; ptr != 0; ptr = ptr->next)
    if (ptr->handler)
      (ptr->handler) (id, evt_args);
}

/// Call handler of type #ON_CHANGE.
/// @param [in] id Game identifier.
/// @param [in] evt_args Event arguments.
static void
sudoku_on_change (uintptr_t id, sudoku_grid_event_args evt_args)
{
  for (sudoku_grid_event_handler_list * ptr = sudokuOnChangeEventHandlers; ptr != 0; ptr = ptr->next)
    if (ptr->handler)
      (ptr->handler) (id, evt_args);
}

/// Call handler of type #ON_SOLVED.
/// @param [in] id Game identifier.
/// @param [in] evt_args Event arguments.
static void
sudoku_on_solved (uintptr_t id, sudoku_grid_event_args evt_args)
{
  for (sudoku_grid_event_handler_list * ptr = sudokuOnSolvedEventHandlers; ptr != 0; ptr = ptr->next)
    if (ptr->handler)
      (ptr->handler) (id, evt_args);
}

/// Definition of message handlers.
typedef struct sudoku_message_handler_list
{
  sudoku_message_handler handler;       ///< Pointer to handler function
  struct sudoku_message_handler_list *next;     ///< Pointer to the next element of the list.
} sudoku_message_handler_list;

/// Handlers to be called on message.
static sudoku_message_handler_list *sudokuOnMessageHandlers;

/// Add handler to the lists of handlers.
/// @param [in] handler Handler to be added.
void
sudoku_message_handler_add (sudoku_message_handler handler)
{
  for (sudoku_message_handler_list * ptr = sudokuOnMessageHandlers; ptr; ptr = ptr->next)
    if (ptr->handler == handler)        // handler already registered
      return;

  sudoku_message_handler_list *const pev = malloc (sizeof (*pev));

  if (pev == 0)
  {
    fprintf (stderr, _("Memory allocation error (%s, %s, %i)\n"), __func__, __FILE__, __LINE__);
    exit (-1);
  }
  pev->handler = handler;
  pev->next = 0;

  if (sudokuOnMessageHandlers == 0)
    sudokuOnMessageHandlers = pev;
  else
  {
    sudoku_message_handler_list *ptr;

    for (ptr = sudokuOnMessageHandlers; ptr->next != 0; ptr = ptr->next)
      /* nothing */ ;
    ptr->next = pev;
  }
}

/// Remove handler to the lists of handlers.
/// @param[in] handler Handler to be removed.
void
sudoku_message_handler_remove (sudoku_message_handler handler)
{
  sudoku_message_handler_list *ptr, *next;

  if (!sudokuOnMessageHandlers)
    return;
  while (sudokuOnMessageHandlers && (!handler || sudokuOnMessageHandlers->handler == handler))
  {
    ptr = sudokuOnMessageHandlers->next;
    free (sudokuOnMessageHandlers);
    sudokuOnMessageHandlers = ptr;
  }
  if (!sudokuOnMessageHandlers)
    return;
  for (ptr = sudokuOnMessageHandlers; ptr && ptr->next;)
  {
    if (ptr->next->handler == handler)
    {
      next = ptr->next->next;
      free (ptr->next);
      ptr->next = next;
    }
    else
      ptr = ptr->next;
  }
}

/// Call handler on message.
/// @param[in] id Game identifier.
/// @param[in] evt_args Event arguments.
static void
sudoku_on_message (uintptr_t id, sudoku_message_args evt_args)
{
  sudoku_message_handler_list *ptr = sudokuOnMessageHandlers;

  for (ptr = sudokuOnMessageHandlers; ptr != 0; ptr = ptr->next)
    if (ptr->handler)
      (ptr->handler) (id, evt_args);
}

/// Constructs message arguments for handler.
/// @param[in] message Message to be emitted.
/// @param[in] verbosity Level of message.
/// @returns message argument.
static sudoku_message_args
get_message_args (const char *message, int verbosity)
{
  sudoku_message_args sudokuMessageArgs;

  sudokuMessageArgs.rule = message;
  sudokuMessageArgs.verbosity = verbosity;
  return (sudokuMessageArgs);
}

/// Clear all event handlers
void
sudoku_all_handlers_clear (void)
{
  sudoku_grid_event_handler_remove (ON_INIT | ON_CHANGE | ON_SOLVED, 0);
  sudoku_message_handler_remove (0);
}

/////////////////////////////////////////////////////////////////////////
///////////////////////////////// internationalization //////////////////
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
///////////////////////////////// ELIMINATION METHOD ////////////////////
/////////////////////////////////////////////////////////////////////////

/// Array of number of bits sets for the integer value of the index of array.
/// @remark `unsigned int` is at least 16 bits in size
static unsigned int NB_BITS[1 << GRID_SIZE];

static unsigned int SUBSETS[1 << GRID_SIZE];
static unsigned int SUBSET_INDEX[GRID_SIZE + 1];

/// Alphabet.
static char ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

/// Digits.
static char DIGIT[] = "123456789abcdefghijklmnopqrstuvwxyz@";

/// Names of values.
static char VALUE_NAME[GRID_SIZE + 1];

/// Code of empty cell
enum
{
  EMPTY_CELL = '0',             // compile-time constant
};

/// Names of rows.
static char ROW_NAME[GRID_SIZE + 1];

/// Names of columns.
static char COLUMN_NAME[GRID_SIZE + 1];

/// Names of regions.
static char REGION_NAME[GRID_SIZE * 3][20];

/// Names of segments.
static char INTERSECTION_NAME[GRID_SIZE * SQUARE_SIZE * 2][50];

const GridReferential sudoku_grid_referential = { ROW_NAME, COLUMN_NAME, VALUE_NAME, EMPTY_CELL };

/// Definition of a cell.
typedef struct
{
  /// Bit mask of possible values in the cell, amongst 9 (size of unsigned is at least 16 bits.)
  unsigned int value;
  /// Cell name
  char name[3];
  /// Set to 1 if the value of the cell is initially set.
  char given;
} cell;

/// Definition of a region.
typedef struct
{
  cell *cell[GRID_SIZE];        ///< The 9 cells of the region (row, column or square).
  int changed;                  ///< Flag inidicating the region has been modified by application of a rule.
  const char *name;             ///< Name of the region, for displaying purpose.
  struct _grid *grid;           ///< owner grid
} region;

/// Definition of a segment (intersection of a square with a line or a column).
// An intersection (iii) between (e.g.) a square and a line
// 'i' indicates the cells at intersection
// '1' indicated the cells outside of the intersection in the square
// '2' indicated the cells outside of the intersection in the line
//
// ... 111 ...
// ... 111 ...
// 222 iii 222
//
// There as many intersections as nb_squares x nb_lines in a square + nb_squares x nb_columns in a square
// i.e. GRID_SIZE * SQUARE_SIZE * 2
typedef struct
{
  cell *r1_cell[GRID_SIZE - SQUARE_SIZE];       ///< The 6 cells of the first region
  cell *r2_cell[GRID_SIZE - SQUARE_SIZE];       ///< The 6 cells of the second region
  int changed;                  ///< Flag inidicating the intersection has been modified by application of a rule.
  const char *name;             ///< Name of the intersection, for displaying purpose.
  struct _grid *grid;           ///< owner grid
} intersection;

/// Definition of a grid.
typedef struct _grid
{
  uintptr_t id;                 ///< Grid identifier
  cell cell[GRID_SIZE][GRID_SIZE];      ///< 81 cells
  intersection intersection[GRID_SIZE * SQUARE_SIZE * 2];       ///< 27 groups of 3 horizontal cells + 27 groups of 3 vertical cells
  region region[GRID_SIZE * 3]; ///< 9 rows, 9 columns, 9 squares
} grid;

/// Definition of counters for statistic purposes.
typedef struct
{
  int nbSolutions;              ///< Number of solutions found
  int nbRules;                  ///< Number of rules
  int backtrackingTries;        ///< Number of backtracking hypothesis
  int backtrackingLevel;        ///< Backtracking depth
  int backtrackingSteps;        ///< Backtracking depth
  int rC[GRID_SIZE];            ///< Number of candidate exclusion per depth
  int rV[GRID_SIZE];            ///< Number of value exclusion per depth
  int rR[GRID_SIZE];            ///< Number of region exclusion per depth
  int rI;                       ///< Number of intersection exclusion
  char theSolution[GRID_SIZE * GRID_SIZE][20];  ///< Last solution found
} counters;

/// Definition of region types.
typedef enum                    // Fixed constant values since the enumeration may be used in arithmetics
{
  ROW = 0,
  COLUMN = 1,
  SQUARE = 2,
} regionType;

/// Converts a bit into a value.
/// @param[in] bit to be converted.
/// @return value
static char
VALUE (unsigned int bit)
{
  if (NB_BITS[bit] != 1)
    return 0;
  else
    for (const char *d = VALUE_NAME; *d && bit; (bit >>= 1), (d++))
      if (bit & 1)
        return *d;

  return EMPTY_CELL;
}

/// Converts a bit pattern into values.
/// @param[in] bits pattern to be converted.
/// @return string of values.
static const char *
VALUES (unsigned int bits)
{
  static char _v[GRID_SIZE * 2];
  int pos = 0;

  for (const char *d = VALUE_NAME; *d && bits; (bits >>= 1), (d++))
    if (bits & 1)
    {
      _v[pos++] = *d;
      _v[pos++] = ' ';
    }

  if (pos)
    pos--;
  _v[pos] = 0;

  return _v;
}

static int grid_cell_changed (grid *, cell *);
static int grid_countEmptyCells (grid *);

/// Eliminates values from an intersection.
/// @param [in,out] inter Intersection
/// @param [in,out] stats Statistic data
/// @return Number of possible values in intersection
static int
intersection_skim (intersection * inter, counters * stats)
{
  unsigned int values[2] = { 0, 0 };
  for (int i = 0; i < GRID_SIZE - SQUARE_SIZE; i++)
  {
    values[0] |= inter->r1_cell[i]->value;
    values[1] |= inter->r2_cell[i]->value;
  }

  unsigned int intersection = values[0] ^ values[1];

  if (intersection)
  {
    stats->nbRules += NB_BITS[intersection];
    stats->rI += NB_BITS[intersection];

    if (sudokuOnMessageHandlers)
    {
      char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

      if (NB_BITS[intersection] > 1)
        MESSAGE_APPEND (rule,
                        _("%s: the values (%s) can only lie in %s.\n"), inter->name, VALUES (intersection),
                        inter->name);
      else
        MESSAGE_APPEND (rule,
                        _("%s: the value (%s) can only lie in %s.\n"), inter->name, VALUES (intersection), inter->name);
      if (*rule)
        sudoku_on_message (inter->grid->id, get_message_args (rule, 1));
    }

    for (int i = 0; i < GRID_SIZE - SQUARE_SIZE; i++)
    {
      unsigned int oldval = inter->r1_cell[i]->value;

      inter->r1_cell[i]->value &= ~intersection;
      if (oldval != inter->r1_cell[i]->value)
        if (grid_cell_changed (inter->grid, inter->r1_cell[i]))
        {
          int nbCells = GRID_SIZE * GRID_SIZE - grid_countEmptyCells (inter->grid);

          sprintf (stats->theSolution[nbCells - 1], "%2i. %s=%c", nbCells, inter->r1_cell[i]->name,
                   VALUE (inter->r1_cell[i]->value));
        }
    }

    for (int i = 0; i < GRID_SIZE - SQUARE_SIZE; i++)
    {
      unsigned int oldval = inter->r2_cell[i]->value;

      inter->r2_cell[i]->value &= ~intersection;
      if (oldval != inter->r2_cell[i]->value)
        if (grid_cell_changed (inter->grid, inter->r2_cell[i]))
        {
          int nbCells = GRID_SIZE * GRID_SIZE - grid_countEmptyCells (inter->grid);

          sprintf (stats->theSolution[nbCells - 1], "%2i. %s=%c", nbCells, inter->r2_cell[i]->name,
                   VALUE (inter->r2_cell[i]->value));
        }
    }
  }

  return NB_BITS[intersection];
}

/// Eliminates regions (rows and columns) for a value
/// @param [in] g Grid
/// @param [in] value Value to be removed.
/// @param [in,out] stats Statistic data
/// @return skimming depth
static int
value_skim (grid * g, unsigned int value, counters * stats)
{
  unsigned int rows;
  unsigned int columns;
  unsigned int bits;

  int stop = 0;

  for (unsigned int depth = 1; depth <= GRID_SIZE && !stop; depth++)
  {
    for (unsigned int index = SUBSET_INDEX[depth - 1]; index < SUBSET_INDEX[depth]; index++)
    {
      bits = SUBSETS[index];

      ////////////////////////////////////////////
      // row exclusion rule
      ////////////////////////////////////////////

      rows = bits;
      columns = 0;
      for (unsigned int row = 0; row < GRID_SIZE; row++)
      {
        if (rows & 1)
          for (unsigned int col = 0; col < GRID_SIZE; col++)
            if (g->cell[row][col].value & (1 << (value - 1)))
              columns |= (1 << col);    // columns of all cells in 'rows' of subset which contain value
        rows >>= 1;
      }

      if (NB_BITS[columns] < NB_BITS[bits])
        return (-1);            // Invalid grid
      else if (NB_BITS[columns] == NB_BITS[bits])
      {
        // other lines than thoses of 'rows' don't contain value in those 'columns'
        unsigned int skimLevel = 0;
        unsigned int otherrows = ~bits;

        for (unsigned int row = 0; row < GRID_SIZE; row++)
        {
          if (otherrows & 1)
          {
            unsigned int cols = columns;

            for (unsigned int col = 0; col < GRID_SIZE; col++)
            {
              if (cols & 1)
              {
                unsigned int oldval = g->cell[row][col].value;

                g->cell[row][col].value &= ~(1 << (value - 1));
                if (oldval != g->cell[row][col].value)
                {
                  skimLevel = NB_BITS[bits];
                  if (grid_cell_changed (g, &g->cell[row][col]))
                  {
                    int nbCells = GRID_SIZE * GRID_SIZE - grid_countEmptyCells (g);

                    sprintf (stats->theSolution[nbCells - 1], "%2i. %s=%c", nbCells, g->cell[row][col].name,
                             VALUE (g->cell[row][col].value));
                  }
                  if (g->cell[row][col].value == 0)
                    return (-1);        // Invalid grid
                }
              }
              cols >>= 1;
            }
          }
          otherrows >>= 1;
        }
        if (skimLevel)
        {
          if (sudokuOnMessageHandlers)
          {
            unsigned int d = 0;
            char noprint = 0;

            if (NB_BITS[bits] == 1)
              for (unsigned int rows = bits; rows; rows >>= 1, d++)
                for (unsigned int col = 0; col < GRID_SIZE; col++)
                  if (g->cell[d][col].value & (1 << (value - 1)) && g->cell[d][col].given)
                    noprint = 1;

            char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

            // Display:
            char row_names[2 * NB_BITS[bits] + 1];
            char col_names[2 * NB_BITS[bits] + 1];

            d = 0;
            for (unsigned int rows = bits; rows; rows >>= 1, d++)
              if (rows & 1)
              {
                row_names[2 * NB_BITS[rows] - 2] = ' ';
                row_names[2 * NB_BITS[rows] - 1] = ROW_NAME[d];
              }
            row_names[2 * NB_BITS[bits]] = '\0';
            d = 0;
            for (unsigned int cols = columns; cols; cols >>= 1, d++)
              if (cols & 1)
              {
                col_names[2 * NB_BITS[cols] - 2] = ' ';
                col_names[2 * NB_BITS[cols] - 1] = COLUMN_NAME[d];
              }
            col_names[2 * NB_BITS[columns]] = '\0';

            if (NB_BITS[bits] > 1)
            {
              MESSAGE_APPEND (rule, _("Value %i in each one of the %i rows [%s] lie only in one of the columns [%s].\n\
-> Value %i in each one of the %i columns [%s] can only lie in the rows [%s].\n"), value, NB_BITS[bits], row_names, col_names, value, NB_BITS[bits], col_names, row_names);

              sudoku_on_message (g->id, get_message_args (rule, 1));
            }
            else if (!noprint)
            {
              MESSAGE_APPEND (rule, _("Value %i in row [%s] lies only in column [%s].\n\
-> Value %i in column [%s] can only lie in the row [%s].\n"), value, row_names, col_names, value, col_names, row_names);

              sudoku_on_message (g->id, get_message_args (rule, 3));
            }
          }

          stats->nbRules++;
          stats->rR[skimLevel - 1]++;
          if (skimLevel > 1)
            return (skimLevel);
          else
            stop = skimLevel;
        }                       // if (skimLevel)
      }                         // if (NB_BITS[values] == NB_BITS[bits])

      ////////////////////////////////////////////
      // column exclusion rule
      ////////////////////////////////////////////

      columns = bits;
      rows = 0;
      for (unsigned int col = 0; col < GRID_SIZE; col++)
      {
        if (columns & 1)
          for (unsigned int row = 0; row < GRID_SIZE; row++)
            if (g->cell[row][col].value & (1 << (value - 1)))
              rows |= (1 << row);       // columns of all cells in 'rows' of subset which contain value
        columns >>= 1;
      }

      if (NB_BITS[rows] < NB_BITS[bits])
        return (-1);            // Invalid grid
      else if (NB_BITS[rows] == NB_BITS[bits])
      {
        // other lines than thoses of 'rows' don't contain value in those 'columns'
        unsigned int skimLevel = 0;
        unsigned int othercols = ~bits;

        for (unsigned int col = 0; col < GRID_SIZE; col++)
        {
          if (othercols & 1)
          {
            unsigned int lrows = rows;

            for (unsigned int row = 0; row < GRID_SIZE; row++)
            {
              if (lrows & 1)
              {
                unsigned int oldval = g->cell[row][col].value;

                g->cell[row][col].value &= ~(1 << (value - 1));
                if (oldval != g->cell[row][col].value)
                {
                  skimLevel = NB_BITS[bits];
                  if (grid_cell_changed (g, &g->cell[row][col]))
                  {
                    int nbCells = GRID_SIZE * GRID_SIZE - grid_countEmptyCells (g);

                    sprintf (stats->theSolution[nbCells - 1], "%2i. %s=%c", nbCells, g->cell[row][col].name,
                             VALUE (g->cell[row][col].value));
                  }
                  if (g->cell[row][col].value == 0)
                    return (-1);        // Invalid grid
                }
              }
              lrows >>= 1;
            }
          }
          othercols >>= 1;
        }
        if (skimLevel)
        {
          if (sudokuOnMessageHandlers)
          {
            unsigned int d = 0;
            char noprint = 0;

            if (NB_BITS[bits] == 1)
              for (unsigned int columns = bits; columns; columns >>= 1, d++)
                for (unsigned int row = 0; row < GRID_SIZE; row++)
                  if (g->cell[row][d].value & (1 << (value - 1)) && g->cell[row][d].given)
                    noprint = 1;

            char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

            // Display:
            char row_names[2 * NB_BITS[bits] + 1];
            char col_names[2 * NB_BITS[bits] + 1];

            d = 0;
            for (unsigned int lrows = rows; lrows; lrows >>= 1, d++)
              if (lrows & 1)
              {
                row_names[2 * NB_BITS[lrows] - 2] = ' ';
                row_names[2 * NB_BITS[lrows] - 1] = ROW_NAME[d];
              }
            row_names[2 * NB_BITS[rows]] = '\0';
            d = 0;
            for (unsigned int cols = bits; cols; cols >>= 1, d++)
              if (cols & 1)
              {
                col_names[2 * NB_BITS[cols] - 2] = ' ';
                col_names[2 * NB_BITS[cols] - 1] = COLUMN_NAME[d];
              }
            col_names[2 * NB_BITS[bits]] = '\0';

            if (NB_BITS[bits] > 1)
            {
              MESSAGE_APPEND (rule, _("Value %i in each one of the %i columns [%s] lie only in one of the rows [%s].\n\
-> Value %i in each one of the %i rows [%s] can only lie in the columns [%s].\n"), value, NB_BITS[bits], col_names, row_names, value, NB_BITS[bits], row_names, col_names);

              sudoku_on_message (g->id, get_message_args (rule, 1));
            }
            else if (!noprint)
            {
              MESSAGE_APPEND (rule, _("Value %i in column [%s] lies only in row [%s].\n\
-> Value %i in row [%s] can only lie in the column [%s].\n"), value, col_names, row_names, value, row_names, col_names);

              sudoku_on_message (g->id, get_message_args (rule, 3));
            }
          }

          stats->nbRules++;
          stats->rR[skimLevel - 1]++;
          if (skimLevel > 1)
            return (skimLevel);
          else
            stop = skimLevel;
        }                       // if (skimLevel)
      }                         // if (NB_BITS[values] == NB_BITS[bits])

    }                           // for (unsigned int index=SUBSET_INDEX[depth-1] ; index<SUBSET_INDEX[depth] ; index++)
  }                             // for (unsigned int depth = 1 ; depth<=9 && !stop ; depth++)

  return stop;
}

/// Eliminates values from a region.
/// @param [in,out] reg Region
/// @param [in,out] stats Statistic data
/// @return Number of possible values in region
static int
region_skim (region * reg, counters * stats)
{
  unsigned int cells;
  unsigned int values;
  unsigned int bits;

  int stop = 0;

  for (unsigned int depth = 1; depth <= GRID_SIZE && !stop; depth++)
  {
    for (unsigned int index = SUBSET_INDEX[depth - 1]; index < SUBSET_INDEX[depth]; index++)
    {
      bits = SUBSETS[index];

      ////////////////////////////////////////////
      // candidate exclusion rule
      ////////////////////////////////////////////

      cells = bits;
      values = 0;
      for (unsigned int cell = 0; cell < GRID_SIZE; cell++)
      {
        if (cells & 1)
          values |= reg->cell[cell]->value;     // values of all cells in 'cells' of subset
        cells >>= 1;
      }

      if (NB_BITS[values] < NB_BITS[bits])
        return (-1);            // Invalid grid
      else if (NB_BITS[values] == NB_BITS[bits])
      {
        // cells of region other than those of 'cells' do not contain values of 'values'
        unsigned int skimLevel = 0;
        unsigned int othercells = ~bits;

        for (unsigned int cell = 0; cell < GRID_SIZE; cell++)
        {
          if (othercells & 1)
          {
            unsigned int oldval = reg->cell[cell]->value;

            reg->cell[cell]->value &= ~values;  // remove values of all cells in 'othercells' of region
            if (oldval != reg->cell[cell]->value)
            {
              skimLevel = NB_BITS[bits];
              if (grid_cell_changed (reg->grid, reg->cell[cell]))
              {
                int nbCells = GRID_SIZE * GRID_SIZE - grid_countEmptyCells (reg->grid);

                sprintf (stats->theSolution[nbCells - 1], "%2i. %s=%c", nbCells, reg->cell[cell]->name,
                         VALUE (reg->cell[cell]->value));
              }
              if (reg->cell[cell]->value == 0)
                return (-1);    // Invalid grid
            }
          }
          othercells >>= 1;
        }
        if (skimLevel)
        {
          if (sudokuOnMessageHandlers)
          {
            unsigned int d = 0;
            char noprint = 0;

            if (NB_BITS[bits] == 1)
              for (unsigned int cells = bits; cells; cells >>= 1, d++)
                if (cells & 1 && reg->cell[d]->given)
                  noprint = 1;

            char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

            // Display:
            char names[GRID_SIZE * 3 + 2] = " ";

            d = 0;
            for (unsigned int cells = bits; cells; cells >>= 1, d++)
              if (cells & 1)
              {
                strcat (names, reg->cell[d]->name);
                strcat (names, " ");
              }

            if (NB_BITS[bits] > 1)
            {
              MESSAGE_APPEND (rule, _("%s: each one of the %i cells [%s] can only accept one of the %i values (%s).\n\
-> %s: each one of the %i values (%s) can only lie in one of the %i cells [%s].\n"), reg->name, NB_BITS[values], names, NB_BITS[values], VALUES (values), reg->name, NB_BITS[values], VALUES (values), NB_BITS[values], names);

              sudoku_on_message (reg->grid->id, get_message_args (rule, 1));
            }
            else if (!noprint)
            {
              MESSAGE_APPEND (rule, _("%s: the cell [%s] can only accept the value (%s).\n\
-> %s: the value (%s) can only lie in the cell [%s].\n"), reg->name, names, VALUES (values), reg->name, VALUES (values), names);

              sudoku_on_message (reg->grid->id, get_message_args (rule, 3));
            }
          }

          stats->nbRules++;
          stats->rC[skimLevel - 1]++;
          if (skimLevel > 1)
            return (skimLevel);
          else
            stop = skimLevel;
        }                       // if (skimLevel)
      }                         // if (NB_BITS[values] == NB_BITS[bits])

      ////////////////////////////////////////////
      // value exclusion rule
      ////////////////////////////////////////////

      cells = 0;
      for (unsigned int cell = GRID_SIZE; cell > 0; cell--)
      {
        cells <<= 1;
        if (bits & reg->cell[cell - 1]->value)  // cell of region contains at least one value in 'bits'
          cells |= 1;
      }

      if (NB_BITS[bits] > NB_BITS[cells])
        return (-1);            // Invalid grid
      else if (NB_BITS[bits] == NB_BITS[cells])
      {
        // values other than those of 'bits' are not contained in cells of cells
        unsigned int tmp = cells;
        unsigned int skimLevel = 0;
        unsigned int othervalues = ~bits;

        for (unsigned int cell = 0; cell < GRID_SIZE; cell++)
        {
          if (cells & 1)
          {
            unsigned int oldval = reg->cell[cell]->value;

            reg->cell[cell]->value &= ~othervalues;     // remove other values of all cells in 'cells' of region
            if (oldval != reg->cell[cell]->value)
            {
              skimLevel = NB_BITS[bits];
              if (grid_cell_changed (reg->grid, reg->cell[cell]))
              {
                int nbCells = GRID_SIZE * GRID_SIZE - grid_countEmptyCells (reg->grid);

                sprintf (stats->theSolution[nbCells - 1], "%2i. %s=%c", nbCells, reg->cell[cell]->name,
                         VALUE (reg->cell[cell]->value));
              }
              if (reg->cell[cell]->value == 0)
                return (-1);    // Invalid grid
            }
          }
          cells >>= 1;
        }
        if (skimLevel)
        {
          if (sudokuOnMessageHandlers)
          {
            unsigned int d = 0;
            char noprint = 0;

            if (NB_BITS[bits] == 1)
              for (unsigned int cells = tmp; cells; cells >>= 1, d++)
                if (cells & 1 && reg->cell[d]->given)
                  noprint = 1;

            char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

            // Display:
            char names[GRID_SIZE * 3 + 2] = " ";

            d = 0;
            for (unsigned int cells = tmp; cells; cells >>= 1, d++)
              if (cells & 1)
              {
                strcat (names, reg->cell[d]->name);
                strcat (names, " ");
              }

            if (NB_BITS[bits] > 1)
            {
              MESSAGE_APPEND (rule, _("%s: each one of the %i values (%s) can only lie in one of the %i cells [%s].\n\
-> %s: each one of the %i cells [%s] can only accept one of the %i values (%s).\n"), reg->name, NB_BITS[bits], VALUES (bits), NB_BITS[bits], names, reg->name, NB_BITS[bits], names, NB_BITS[bits], VALUES (bits));

              sudoku_on_message (reg->grid->id, get_message_args (rule, 1));
            }
            else if (!noprint)
            {
              MESSAGE_APPEND (rule, _("%s: the value (%s) can only lie in the cell [%s].\n\
-> %s: the cell [%s] can only accept the value (%s).\n"), reg->name, VALUES (bits), names, reg->name, names, VALUES (bits));

              sudoku_on_message (reg->grid->id, get_message_args (rule, 2));
            }
          }

          stats->nbRules++;
          stats->rV[skimLevel - 1]++;
          if (skimLevel > 1)
            return (skimLevel);
          else
            stop = skimLevel;
        }                       // if (skimLevel)
      }                         // if (NB_BITS[bits] == NB_BITS[cells])
    }                           // for (unsigned int index=SUBSET_INDEX[depth-1] ; index<SUBSET_INDEX[depth] ; index++)
  }                             // for (unsigned int depth = 1 ; depth<=9 && !stop ; depth++)

  return stop;
}

/// Counts empty cells.
/// @param[in] g grid
/// @return Number of empty cells
static int
grid_countEmptyCells (grid * g)
{
  int ret = GRID_SIZE * GRID_SIZE;

  for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++)
    if (NB_BITS[g->cell[i / GRID_SIZE][i % GRID_SIZE].value] == 1)
      ret--;

  return (ret);
}

/// Constructs event arguments for handler.
/// @param[in] g grid
/// @returns event argument.
static sudoku_grid_event_args
get_event_args (grid * g)
{
  sudoku_grid_event_args sudokuOnEventArgs;

  for (int r = 0; r < GRID_SIZE; r++)
    for (int c = 0; c < GRID_SIZE; c++)
      for (int v = 0; v < GRID_SIZE; v++)
        if (g->cell[r][c].value & (1 << v))
          sudokuOnEventArgs.grid[r][c][v] = v + 1;
        else
          sudokuOnEventArgs.grid[r][c][v] = 0;

  sudokuOnEventArgs.nbCells = GRID_SIZE * GRID_SIZE - grid_countEmptyCells (g);

  return (sudokuOnEventArgs);
}

/// Initialize a grid.
/// @param[in] g Grid
static void
grid_init (grid * g)
{
  for (int r = 0; r < GRID_SIZE * 3; r++)       // 27 regions
  {
    g->region[r].grid = g;

    regionType t = r / GRID_SIZE;

    switch (t)
    {
      case ROW:
        for (int c = 0; c < GRID_SIZE; c++)     // 9 cells of region
          g->region[r].cell[c] = &(g->cell[r % GRID_SIZE][c]);
        break;
      case COLUMN:
        for (int c = 0; c < GRID_SIZE; c++)     // 9 cells of region
          g->region[r].cell[c] = &(g->cell[c][r % GRID_SIZE]);
        break;
      case SQUARE:
        for (int c = 0; c < GRID_SIZE; c++)     // 9 cells of region
          g->region[r].cell[c] =
            &(g->cell[c / SQUARE_SIZE + SQUARE_SIZE * ((r % GRID_SIZE) / SQUARE_SIZE)][c % SQUARE_SIZE +
                                                                                       SQUARE_SIZE * (r %
                                                                                                      SQUARE_SIZE)]);
        break;
    }
  }

  for (int i = 0; i < GRID_SIZE * SQUARE_SIZE * 2; i++) // 54 intersections
  {
    g->intersection[i].grid = g;

    regionType direction = i / (GRID_SIZE * SQUARE_SIZE);
    int inter = i % (GRID_SIZE * SQUARE_SIZE);

    switch (direction)
    {
      case COLUMN:
        for (int j = 0; j < SQUARE_SIZE; j++)
        {
          int r = inter / SQUARE_SIZE;
          int c = (SQUARE_SIZE * inter + j) % GRID_SIZE;

          for (int k = 0; k < SQUARE_SIZE - 1; k++)
          {
            g->intersection[i].r1_cell[j + SQUARE_SIZE * k] = &(g->cell[r][(c + SQUARE_SIZE * (k + 1)) % GRID_SIZE]);
            g->intersection[i].r2_cell[j + SQUARE_SIZE * k] =
              &(g->cell[SQUARE_SIZE * (r / SQUARE_SIZE) + (r + k + 1) % SQUARE_SIZE][c]);
          }
        }
        break;
      case ROW:
        for (int j = 0; j < SQUARE_SIZE; j++)
        {
          int r = SQUARE_SIZE * (inter / GRID_SIZE) + j;
          int c = inter % GRID_SIZE;

          for (int k = 0; k < SQUARE_SIZE - 1; k++)
          {
            g->intersection[i].r1_cell[j + SQUARE_SIZE * k] = &(g->cell[(r + SQUARE_SIZE * (k + 1)) % GRID_SIZE][c]);
            g->intersection[i].r2_cell[j + SQUARE_SIZE * k] =
              &(g->cell[r][SQUARE_SIZE * (c / SQUARE_SIZE) + (c + k + 1) % SQUARE_SIZE]);
          }
        }
        break;
      case SQUARE:
        exit (-1);
        break;
    }
  }
}

/// Tag cell as changed.
/// @param [in] g Grid
/// @param [in] cell Cell changed
/// @return 1 if cell has been filled, 0 otherwise
static int
grid_cell_changed (grid * g, cell * cell)
{
  for (int ir = 0; ir < GRID_SIZE * 3; ir++)
  {
    if (g->region[ir].changed)
      continue;

    for (int c = 0; c < GRID_SIZE; c++)
      if (g->region[ir].cell[c] == cell)
      {
        g->region[ir].changed = 1;
        break;
      }
  }

  for (int ir = 0; ir < GRID_SIZE * SQUARE_SIZE * 2; ir++)
  {
    if (g->intersection[ir].changed)
      continue;

    for (int c = 0; c < (GRID_SIZE - SQUARE_SIZE); c++)
      if (g->intersection[ir].r1_cell[c] == cell || g->intersection[ir].r2_cell[c] == cell)
      {
        g->intersection[ir].changed = 1;
        break;
      }
  }

  if (NB_BITS[cell->value] == 1)
  {
    int nbCells = GRID_SIZE * GRID_SIZE - grid_countEmptyCells (g);

    if (sudokuOnMessageHandlers)
    {
      char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

      MESSAGE_APPEND (rule, _("\n  ### Cell %s must contain %c [%2i] ###\n\n"), cell->name, VALUE (cell->value),
                      nbCells);
      if (*rule)
        sudoku_on_message (0, get_message_args (rule, 1));
    }
    return 1;
  }
  else
    return 0;
}

/// Initialize a cell state.
/// @param[in] g Grid
/// @param[in] c Column
/// @param[in] l Line
/// @param[in] v Value
static void
grid_initCell (grid * g, int l, int c, int v)
{
  if (l < 0 || l >= GRID_SIZE || c < 0 || c >= GRID_SIZE || v < 0 || v > GRID_SIZE)
    return;

  static unsigned int all = (1 << GRID_SIZE) - 1;

  if (v == 0)
  {
    g->cell[l][c].value = all;
    g->cell[l][c].given = 0;
  }
  else
  {
    g->cell[l][c].value = (1 << (v - 1));
    g->cell[l][c].given = 1;
  }
}

/// Initialize a grid from an array.
/// @param[in] g Grid
/// @param[in] intg Array
static void
grid_init_from_int9x9 (grid * g, int intg[GRID_SIZE][GRID_SIZE])
{
  grid_init (g);

  for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++)
    grid_initCell (g, i / GRID_SIZE, i % GRID_SIZE, intg[i / GRID_SIZE][i % GRID_SIZE]);

  for (int r = 0; r < GRID_SIZE; r++)   // 9 rows
    for (int c = 0; c < GRID_SIZE; c++) // 9 colmuns
    {
      g->cell[r][c].name[0] = ROW_NAME[r];
      g->cell[r][c].name[1] = COLUMN_NAME[c];
      g->cell[r][c].name[2] = 0;
    }

  for (int ir = 0; ir < GRID_SIZE * 3; ir++)
  {
    g->region[ir].changed = 1;
    g->region[ir].name = REGION_NAME[ir];
  }

  for (int ir = 0; ir < GRID_SIZE * SQUARE_SIZE * 2; ir++)
  {
    g->intersection[ir].changed = 1;
    g->intersection[ir].name = INTERSECTION_NAME[ir];
  }

  g->id = (uintptr_t) g;
}

/// Copy a grid.
/// @param[in] dest Destination grid
/// @param[in] src Source grid
static void
grid_copy (grid * dest, grid * src)
{
  if (dest == src)
  {
    fprintf (stderr, _("Unexpected error (%s, %s, %i).\n"), __func__, __FILE__, __LINE__);
    exit (-1);
  }

  *dest = *src;
  grid_init (dest);
}

/// Eliminates regions.
/// @param [in] g Grid
/// @param [out] stats Statistic data
/// @return 1 if some candidates have been excluded, 0 otherwise, -1 if g is invalide
static int
grid_skimValues (grid * g, counters * stats)
{
  int gridSkimmed = 0;

  for (unsigned int value = 1; value <= GRID_SIZE; value++)
  {
    int ret = value_skim (g, value, stats);

    if (ret > 0)
    {
      if (ret > gridSkimmed)
        gridSkimmed = ret;

      if ( /*ret > 1 && */ sudokuOnChangeEventHandlers)
        sudoku_on_change (g->id, get_event_args (g));
    }                           // if (ret>0)
    else if (ret < 0)
    {
      if (sudokuOnMessageHandlers)
      {
        char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

        MESSAGE_APPEND (rule, _("  => Invalid grid.\n"));
        sudoku_on_message (g->id, get_message_args (rule, 1));
      }
      return (ret);             // Invalid grid
    }                           // if (ret<0)
  }

  return (gridSkimmed);
}

/// Eliminates candidates in regions.
/// @param [in] g Grid
/// @param [out] stats Statistic data
/// @return 1 if some candidates have been excluded, 0 otherwise, -1 if g is invalide
static int
grid_skimRegions (grid * g, counters * stats)
{
  int gridSkimmed = 0;

  for (int ir = 0; ir < GRID_SIZE * 3; ir++)
  {
    if (g->region[ir].changed == 0)
      continue;

    g->region[ir].changed = 0;
    int ret = region_skim (&(g->region[ir]), stats);

    if (ret > 0)
    {
      if (ret > gridSkimmed)
        gridSkimmed = ret;

      if ( /*ret > 1 && */ sudokuOnChangeEventHandlers)
        sudoku_on_change (g->id, get_event_args (g));
    }                           // if (ret>0)
    else if (ret < 0)
    {
      if (sudokuOnMessageHandlers)
      {
        char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

        MESSAGE_APPEND (rule, _("  => Invalid grid.\n"));
        sudoku_on_message (g->id, get_message_args (rule, 1));
      }
      return (ret);             // Invalid grid
    }                           // if (ret<0)
  }                             // loop on 27 regions
  return (gridSkimmed);
}

/// Eliminates candidates in regions.
/// @param [in] g Grid
/// @param [out] stats Statistic data
/// @return 1 if some candidates have been excluded, 0 otherwise, -1 if g is invalide
static int
grid_skimIntersections (grid * g, counters * stats)
{
  int gridSkimmed = 0;

  for (int ir = 0; ir < GRID_SIZE * SQUARE_SIZE * 2; ir++)
  {
    if (g->intersection[ir].changed == 0)
      continue;

    g->intersection[ir].changed = 0;
    int ret = intersection_skim (&(g->intersection[ir]), stats);

    if (ret > 0)
    {
      gridSkimmed += ret;
      if (sudokuOnChangeEventHandlers)
        sudoku_on_change (g->id, get_event_args (g));
    }
  }
  return gridSkimmed;
}

/// Solves a grid.
/// @param [in] g Grid
/// @param [in] find \c FIRST to find the first solution or \c ALL to find all solutions
/// @param [out] stats Statistic data
/// @return 1 if some candidates have been excluded, 0 otherwise, -1 if g is invalide
static int
grid_solveByElimination (grid * g, findSolutions find, counters * stats)
{
  int skim = 1;

  while (skim > 0)
  {
    // skim regions
    int r = grid_skimRegions (g, stats);

    skim = r;
    if (r < 0)
      return (-1);
    else if (r)
      continue;

    // skim values
    r = grid_skimValues (g, stats);
    skim = r;
    if (r < 0)
      return (-1);
    else if (r)
      continue;

    // skim intersections
    int i = grid_skimIntersections (g, stats);

    if (i < 0)
      return (-1);

    skim += i;
  }                             // while ( skim > 0 )

  // MAKE HYPOTHESIS :
  // the grid is valid but :
  // no skim done -> needs hypothesis (backtracking, recursive call to grid_solveByElimination)
  int ipivot = -1;
  unsigned int min = 10;

  for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++)
  {
    unsigned int j = NB_BITS[g->cell[i / GRID_SIZE][i % GRID_SIZE].value];

    if (j >= 2 && j < min)
    {
      ipivot = i;
      min = j;
      if (min == 2)
        break;
    }
  }

  if (ipivot >= 0)
  {
    if (sudokuOnChangeEventHandlers)
      sudoku_on_change (g->id, get_event_args (g));

    int retCode = -1;
    unsigned int value = 1;

    for (unsigned int bits = g->cell[ipivot / GRID_SIZE][ipivot % GRID_SIZE].value; bits != 0; bits >>= 1, value <<= 1)
    {
      if (!(bits & 1))
        continue;

      grid clone;

      grid_copy (&clone, g);
      cell *pivot = &(clone.cell[ipivot / GRID_SIZE][ipivot % GRID_SIZE]);

      pivot->value = value;     // cell modified here

      //stats->nbSteps++ ;
      int nbCells = GRID_SIZE * GRID_SIZE - grid_countEmptyCells (&clone);

      sprintf (stats->theSolution[nbCells - 1], "%2i. %s=%c?", nbCells, pivot->name, VALUE (pivot->value));
      if (sudokuOnMessageHandlers)
      {
        char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

        MESSAGE_APPEND (rule, _("  ??? Hypothesis: cell %s = %c ? (out of %s) [%2i] ???\n"),
                        pivot->name, VALUE (pivot->value),
                        VALUES (g->cell[ipivot / GRID_SIZE][ipivot % GRID_SIZE].value), nbCells);
        //MESSAGE_APPEND (rule, "  [%2i]\n", nbCells);
        sudoku_on_message (g->id, get_message_args (rule, 1));
      }

      grid_cell_changed (&clone, pivot);

      stats->backtrackingTries++;
      stats->backtrackingLevel++;
      int k = grid_solveByElimination (&clone, find, stats);
      int nbSteps = grid_countEmptyCells (g) - grid_countEmptyCells (&clone);

      if (nbSteps > stats->backtrackingSteps)
        stats->backtrackingSteps = nbSteps;
      if (k > 0)
      {
        stats->backtrackingLevel = retCode = k;
        if (find == FIRST)
          return (k);           // don't go further the first solution found
      }
      else if (k == 0)
      {
        fprintf (stderr, _("Unexpected error (%s, %s, %i).\n"), __func__, __FILE__, __LINE__);
        exit (-1);
      }
      else                      // if (k<0)
      {                         // Invalid guess
        stats->backtrackingLevel--;
        if (sudokuOnMessageHandlers)
        {
          char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

          MESSAGE_APPEND (rule,
                          _("  %%%%%% Incorrect guess: cell %s = %c [%2i] (after %i steps). %%%%%%\n"), pivot->name,
                          VALUE (pivot->value), nbCells, nbSteps);
          sudoku_on_message (g->id, get_message_args (rule, 1));
        }
      }
    }
    return (retCode);
  }
  else                          // the grid is complete and valid
  {
    stats->nbSolutions++;
    if (sudokuOnMessageHandlers)
    {
      char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

      MESSAGE_APPEND (rule, _("Solved using elimination method (solution #%i).\n"), stats->nbSolutions);

      for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++)
        if (stats->theSolution[i][0] != 0)
          MESSAGE_APPEND (rule, "%s%c", stats->theSolution[i], ((i + 1) % SQUARE_SIZE ? '\t' : '\n'));

      MESSAGE_APPEND (rule, "\n");
      sudoku_on_message (g->id, get_message_args (rule, 0));
    }
    if (sudokuOnSolvedEventHandlers)
      sudoku_on_solved (g->id, get_event_args (g));

    return (stats->backtrackingLevel);
  }
}

/////////////////////////////////////////////////////////////////////////
///////////////////////////////// BACKTRACKING METHOD ///////////////////
/////////////////////////////////////////////////////////////////////////
/// Construct event argument from an array.
/// @param[in] g array grid
static sudoku_grid_event_args
int9x9_print (int g[GRID_SIZE][GRID_SIZE])
{
  int nbc = 0;

  for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++)
    if (g[i / GRID_SIZE][i % GRID_SIZE])
      nbc++;

  sudoku_grid_event_args sudokuOnEventArgs;

  for (int r = 0; r < GRID_SIZE; r++)
    for (int c = 0; c < GRID_SIZE; c++)
    {
      sudokuOnEventArgs.grid[r][c][0] = g[r][c];
      for (int v = 1; v < GRID_SIZE; v++)
        sudokuOnEventArgs.grid[r][c][v] = 0;
    }

  sudokuOnEventArgs.nbCells = nbc;

  return (sudokuOnEventArgs);
}

/// Check that the grid is valid.
/// @param[in] g array grid
/// @return 1 if grid is valid
static int
int9x9_check (int g[GRID_SIZE][GRID_SIZE])
{
  regionType t[3] = { ROW, COLUMN, SQUARE };

  for (int row = 0; row < GRID_SIZE; row++)
    for (int column = 0; column < GRID_SIZE; column++)
      if (g[row][column] != 0)
        for (int cell = 0; cell < GRID_SIZE; cell++)
          for (int ir = 0; ir < 3; ir++)
            switch (t[ir])
            {
              case COLUMN:
                if ((cell != row) && (g[cell][column] == g[row][column]))
                  return 0;
                break;
              case ROW:
                if ((cell != column) && (g[row][cell] == g[row][column]))
                  return 0;
                break;
              case SQUARE:
                if ((SQUARE_SIZE * (row / SQUARE_SIZE) + cell / SQUARE_SIZE != row) &&
                    (SQUARE_SIZE * (column / SQUARE_SIZE) + cell % SQUARE_SIZE) != column &&
                    (g[SQUARE_SIZE * (row / SQUARE_SIZE) + cell / SQUARE_SIZE]
                     [SQUARE_SIZE * (column / SQUARE_SIZE) + cell % SQUARE_SIZE] == g[row][column]))
                  return 0;
                break;
            }

  return 1;
}

/// Solves a grid using backtracking.
/// @param[in] id Grid idenitifier
/// @param[in] g array grid
/// @param[in] find \c FIRST to find the first solution or \c ALL to find all solutions
/// @param[out] stats Statistic data
/// @return 1 if some candidates have been excluded, 0 otherwise, -1 if g is invalide
static int
int9x9_solveByBacktracking (uintptr_t id, int g[GRID_SIZE][GRID_SIZE], findSolutions find, counters * stats)
{
  int retCode = 0;
  int ii;

  for (ii = 0; ii < GRID_SIZE * GRID_SIZE && g[ii / GRID_SIZE][ii % GRID_SIZE] != 0; ii++)
    ;

  if (ii < GRID_SIZE * GRID_SIZE)
  {
    int l = ii / GRID_SIZE;
    int c = ii % GRID_SIZE;

    for (int value = 1; value <= GRID_SIZE; value++)
    {
      int skipValue = 0;

      for (int cell = 0; cell < GRID_SIZE; cell++)
        if (g[l][cell] == value || g[cell][c] == value
            || g[SQUARE_SIZE * (l / SQUARE_SIZE) + cell / SQUARE_SIZE][SQUARE_SIZE * (c / SQUARE_SIZE) +
                                                                       cell % SQUARE_SIZE] == value)
          skipValue = 1;

      if (!skipValue)
      {
        int clone[GRID_SIZE][GRID_SIZE];

        memcpy (clone, g, GRID_SIZE * GRID_SIZE * sizeof (int));
        clone[l][c] = value;
        stats->backtrackingTries++;
        int i = int9x9_solveByBacktracking (id, clone, find, stats);

        if (i > 0)
        {
          retCode = 1;
          if (find == FIRST)
            return (1);         // don't go further the first solution found
        }
      }
    }

    //stats->nbSteps++;
    return (retCode);
  }
  else
  {
    stats->nbSolutions++;
    char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

    MESSAGE_APPEND (rule, _("Solved using backtracking method (solution #%i, %i tries).\n"), stats->nbSolutions,
                    stats->backtrackingTries);
    sudoku_on_message (id, get_message_args (rule, 0));
    sudoku_on_solved (id, int9x9_print (g));
    return (1);
  }
}

/// Initialize static data.
void
sudoku_init (void)
{
  // Internationalization with gettext
  bindtextdomain (PACKAGE, "./po");

  NB_BITS[0] = 0;
  for (unsigned int i = 1; i < (1 << GRID_SIZE); i++)
    NB_BITS[i] = (i & 1) + NB_BITS[i >> 1];

  for (int i = 0; i < GRID_SIZE; i++)
  {
    VALUE_NAME[i] = DIGIT[i];
    ROW_NAME[i] = toupper (ALPHABET[i]);
    COLUMN_NAME[i] = tolower (ALPHABET[i + (GRID_SIZE <= 9 ? GRID_SIZE : 0)]);
  }

  for (int r = 0; r < GRID_SIZE * 3; r++)       // 27 regions
  {
    regionType t = r / GRID_SIZE;

    switch (t)
    {
      case ROW:
        snprintf (REGION_NAME[r], sizeof (REGION_NAME[r]), _("Row %c"), ROW_NAME[r % GRID_SIZE]);
        break;
      case COLUMN:
        snprintf (REGION_NAME[r], sizeof (REGION_NAME[r]), _("Column %c"), COLUMN_NAME[r % GRID_SIZE]);
        break;
      case SQUARE:
        snprintf (REGION_NAME[r], sizeof (REGION_NAME[r]), _("Square %c%c-%c%c"),
                  ROW_NAME[SQUARE_SIZE * ((r % GRID_SIZE) / SQUARE_SIZE)], COLUMN_NAME[SQUARE_SIZE * (r % SQUARE_SIZE)],
                  ROW_NAME[SQUARE_SIZE * ((r % GRID_SIZE) / SQUARE_SIZE) + SQUARE_SIZE - 1],
                  COLUMN_NAME[SQUARE_SIZE * (r % SQUARE_SIZE) + SQUARE_SIZE - 1]);
        break;
    }
  }

  for (int i = 0; i < GRID_SIZE * SQUARE_SIZE * 2; i++) // 54 intersections
  {
    regionType direction = i / (GRID_SIZE * SQUARE_SIZE);
    int inter = i % (GRID_SIZE * SQUARE_SIZE);

    switch (direction)
    {
      case COLUMN:
        snprintf (INTERSECTION_NAME[i], sizeof (INTERSECTION_NAME[i]), _("Segment %c%c-%c%c"),
                  ROW_NAME[inter / SQUARE_SIZE], COLUMN_NAME[(SQUARE_SIZE * inter) % GRID_SIZE],
                  ROW_NAME[inter / SQUARE_SIZE], COLUMN_NAME[(SQUARE_SIZE * inter + SQUARE_SIZE - 1) % GRID_SIZE]);
        break;
      case ROW:
        snprintf (INTERSECTION_NAME[i], sizeof (INTERSECTION_NAME[i]), _("Segment %c%c-%c%c"),
                  ROW_NAME[SQUARE_SIZE * (inter / GRID_SIZE)], COLUMN_NAME[inter % GRID_SIZE],
                  ROW_NAME[SQUARE_SIZE * (inter / GRID_SIZE) + SQUARE_SIZE - 1], COLUMN_NAME[inter % GRID_SIZE]);
        break;
      case SQUARE:
        break;
    }
  }

  unsigned int index = 0;

  for (int i = 0; i <= GRID_SIZE; i++)
  {
    for (unsigned int j = 0; j < (1 << GRID_SIZE); j++)
      if (NB_BITS[j] == i)
      {
        SUBSETS[index++] = j;
        SUBSET_INDEX[i] = index;
      }
  }
}

/////////////////////////////////////////////////////////////////////////
////////////////////////// EXACT COVER SEARCH METHOD ////////////////////
/////////////////////////////////////////////////////////////////////////

/// Displayer function to be used by the exqct cover serach library libdlx.a.
/// @see [dancing links library](https://github.com/farhiongit/dancing-links).
static void
exact_cover_search_solution_displayer (Universe head, unsigned long length, const char *const *solution, void *ptr)
{
  if (!length || !solution)
  {
    sudoku_on_message ((uintptr_t) head, get_message_args (_("Grid is not valid.\n"), 0));
    return;
  }

  int g[GRID_SIZE][GRID_SIZE];

  for (unsigned long i = 0; i < length; i++)
    if (solution[i] && strchr (DIGIT, solution[i][1]) && strchr (DIGIT, solution[i][3])
        && strchr (DIGIT, solution[i][5]))
      g[strchr (DIGIT, solution[i][1]) - DIGIT][strchr (DIGIT, solution[i][3]) - DIGIT] =
        strchr (DIGIT, solution[i][5]) - DIGIT + 1;

  sudoku_on_solved ((uintptr_t) head, int9x9_print (g));
}

/* Interface function */
/// Get version.
/// @return version
const char *
sudoku_get_version (void)
{
  static char *version = 0;

  if (version == 0)
  {
    int length = snprintf (version, 0, "V%s, %s %s", SUDOKU_SOLVE_VERSION, __DATE__, __TIME__) + 1;

    version = malloc (length * sizeof (*version));
    if (version == 0)
    {
      fprintf (stderr, _("Memory allocation error (%s, %s, %i)\n"), __func__, __FILE__, __LINE__);
      exit (-1);
    }
    snprintf (version, length, "V%s, %s %s", SUDOKU_SOLVE_VERSION, __DATE__, __TIME__);
    sudoku_on_message (0, get_message_args (version, 0));
  }
  return version;
}

method
sudoku_solve (int g[GRID_SIZE][GRID_SIZE], method method, findSolutions find)
{
  sudoku_init ();

  for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++)
    if (g[i / GRID_SIZE][i % GRID_SIZE] < 0 || g[i / GRID_SIZE][i % GRID_SIZE] > GRID_SIZE)
    {
      if (sudokuOnMessageHandlers)
      {
        char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

        MESSAGE_APPEND (rule, _("Grid is not valid.\n"));
        sudoku_on_message (0, get_message_args (rule, 0));
      }
      return (NONE);
    }

  counters theStats;

  theStats.nbSolutions = theStats.nbRules = theStats.backtrackingLevel =
    theStats.backtrackingSteps = theStats.backtrackingTries = theStats.rI = 0;
  for (int i = 0; i < GRID_SIZE; i++)
    theStats.rC[i] = theStats.rV[i] = 0;

  // USING ELIMINATION METHOD
  if (method == ELIMINATION)
  {
    // the sudoku grid to solve
    grid theGridCells;

    grid_init_from_int9x9 (&theGridCells, g);

    if (sudokuOnInitEventHandlers)
      sudoku_on_init (theGridCells.id, get_event_args (&theGridCells));

    // Solve
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++)
      theStats.theSolution[i][0] = 0;

    // Searching for solutions.
    int ret = grid_solveByElimination (&theGridCells, find, &theStats);

    // Clean after yourself

    if (ret < 0)
    {
      if (sudokuOnMessageHandlers)
      {
        char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

        MESSAGE_APPEND (rule, _("Grid is not valid.\n"));
        sudoku_on_message (theGridCells.id, get_message_args (rule, 0));
      }
      ret = 0;
    }
    else
    {
      ret = 1;
      if (sudokuOnMessageHandlers)
      {
        char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";

        MESSAGE_APPEND (rule, ngettext ("%i solution found.\n", "%i solutions found.\n", theStats.nbSolutions),
                        theStats.nbSolutions);
        MESSAGE_APPEND (rule, _("Solved with %i rules and %i hypothesis.\n"), theStats.nbRules,
                        theStats.backtrackingTries);
        MESSAGE_APPEND (rule, _("Cell Exclusion:\n"));
        for (int i = GRID_SIZE; i > 0; i--)
          if (theStats.rC[i - 1] > 0)
            MESSAGE_APPEND (rule, _("\tDepth %i: %i\n"), i, theStats.rC[i - 1]);
        MESSAGE_APPEND (rule, _("Candidate Exclusion:\n"));
        for (int i = GRID_SIZE; i > 0; i--)
          if (theStats.rV[i - 1] > 0)
            MESSAGE_APPEND (rule, _("\tDepth %i: %i\n"), i, theStats.rV[i - 1]);
        MESSAGE_APPEND (rule, _("Value Exclusion:\n"));
        for (int i = GRID_SIZE; i > 0; i--)
          if (theStats.rR[i - 1] > 0)
            MESSAGE_APPEND (rule, _("\tDepth %i: %i\n"), i, theStats.rR[i - 1]);
        MESSAGE_APPEND (rule, _("Regions Exclusion:\n"));
        MESSAGE_APPEND (rule, _("\t%i\n"), theStats.rI);
        MESSAGE_APPEND (rule, _("Backtracking:\n"));
        MESSAGE_APPEND (rule, _("\tDepth: %i\n"), theStats.backtrackingLevel);
        MESSAGE_APPEND (rule, _("\tSteps: %i\n"), theStats.backtrackingSteps);
        MESSAGE_APPEND (rule, _("\tHypothesis: %i\n"), theStats.backtrackingTries);
        sudoku_on_message (theGridCells.id, get_message_args (rule, 0));
      }
    }
    return (theStats.backtrackingTries ? BACKTRACKING : ELIMINATION);
  }

  // USING BACKTRACKING METHOD
  else if (method == BACKTRACKING)
  {
    uintptr_t gridID = (uintptr_t) g;

    sudoku_on_init (gridID, int9x9_print (g));

    // Searching for solutions.
    if (int9x9_check (g) == 0 || int9x9_solveByBacktracking (gridID, g, find, &theStats) == 0)
    {
      sudoku_on_message (gridID, get_message_args (_("Grid is not valid.\n"), 0));
      return (NONE);
    }
    else
      return (BACKTRACKING);
  }

  // USING EXACT COVER METHOD
  else if (method == EXACT_COVER)
  {
    char cell[] = "R?C?#?";
    char inCell[] = "R?C?";
    char inRow[] = "R?#?";
    char inColumn[] = "C?#?";
    char inBox[] = "B?#?";

    // Initialize the columns of the matrix to be covered exactly.
    char columns[strlen (inCell) * (GRID_SIZE * GRID_SIZE) + (GRID_SIZE * GRID_SIZE) +
                 strlen (inRow) * (GRID_SIZE * GRID_SIZE) + (GRID_SIZE * GRID_SIZE) +
                 strlen (inColumn) * (GRID_SIZE * GRID_SIZE) + (GRID_SIZE * GRID_SIZE) +
                 strlen (inBox) * (GRID_SIZE * GRID_SIZE) + (GRID_SIZE * GRID_SIZE) + 1];
    *columns = 0;
    for (int i = 1; i <= GRID_SIZE; i++)
      for (int j = 1; j <= GRID_SIZE; j++)
      {
        inCell[1] = DIGIT[i - 1];
        inCell[3] = DIGIT[j - 1];
        strcat (columns, inCell);
        strcat (columns, "|");

        inRow[1] = DIGIT[i - 1];
        inRow[3] = DIGIT[j - 1];
        strcat (columns, inRow);
        strcat (columns, "|");

        inColumn[1] = DIGIT[i - 1];
        inColumn[3] = DIGIT[j - 1];
        strcat (columns, inColumn);
        strcat (columns, "|");

        inBox[1] = DIGIT[i - 1];
        inBox[3] = DIGIT[j - 1];
        strcat (columns, inBox);
        strcat (columns, "|");
      }

    // Initialize a matrix to be covered exactly.
    Universe sudoku = dlx_universe_create (columns, "|");

    dlx_displayer_set (sudoku, exact_cover_search_solution_displayer, 0);
    //(void) (exact_cover_search_solution_displayer);

    sudoku_on_init ((uintptr_t) sudoku, int9x9_print (g));

    // Initialize the lines of the matrix to be covered exactly.
    char line[strlen (inCell) + 1 + strlen (inRow) + 1 + strlen (inColumn) + 1 + strlen (inBox) + 1];

    for (int row = 1; row <= GRID_SIZE; row++)
      for (int column = 1; column <= GRID_SIZE; column++)
        for (int number = 1; number <= GRID_SIZE; number++)
        {
          cell[1] = DIGIT[row - 1];
          cell[3] = DIGIT[column - 1];
          cell[5] = DIGIT[number - 1];

          *line = 0;
          inCell[1] = DIGIT[row - 1];
          inCell[3] = DIGIT[column - 1];
          strcat (line, inCell);
          strcat (line, "|");

          inRow[1] = DIGIT[row - 1];
          inRow[3] = DIGIT[number - 1];
          strcat (line, inRow);
          strcat (line, "|");

          inColumn[1] = DIGIT[column - 1];
          inColumn[3] = DIGIT[number - 1];
          strcat (line, inColumn);
          strcat (line, "|");

          inBox[1] = DIGIT[SQUARE_SIZE * ((row - 1) / SQUARE_SIZE) + (column - 1) / SQUARE_SIZE];
          inBox[3] = DIGIT[number - 1];
          strcat (line, inBox);

          dlx_subset_define (sudoku, cell, line, "|");
        }

    // Initialize the already covered lines of the matrix to be covered exactly
    // using the initial sudoku grid (g).
    for (int row = 1; row <= GRID_SIZE; row++)
      for (int column = 1; column <= GRID_SIZE; column++)
        if (g[row - 1][column - 1])
        {
          cell[1] = DIGIT[row - 1];
          cell[3] = DIGIT[column - 1];
          cell[5] = DIGIT[g[row - 1][column - 1] - 1];
          if (!dlx_subset_require_in_solution (sudoku, cell))
          {
            sudoku_on_message ((uintptr_t) sudoku, get_message_args (_("Grid is not valid.\n"), 0));
            return (NONE);
          }
        }

    // Searching for solutions covering exactly the matrix.
    char rule[SUDOKU_MAX_MESSAGE_LENGTH] = "";
    unsigned long nbsol = dlx_exact_cover_search (sudoku, find == FIRST ? 1 : 0);

    MESSAGE_APPEND (rule, ngettext ("%i solution found.\n", "%i solutions found.\n", nbsol), nbsol);
    MESSAGE_APPEND (rule, _("Solved using exact cover search method.\n"));
    sudoku_on_message ((uintptr_t) sudoku, get_message_args (rule, 0));

    // Freeing all.
    dlx_universe_destroy (sudoku);
  }
  return (NONE);
}
