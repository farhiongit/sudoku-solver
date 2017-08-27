
/**
 * @file
 * Sudoku solver interface.
 */

/***********************************************************************************
* Author: Laurent Farhi                                                            *
* Name: solve.h                                                                    *
* Language: C                                                                      *
* Copyright (C) 2009, All rights reserved.                                         *
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

#pragma once
#ifndef SUDOKU_SOLVE_H
#define SUDOKU_SOLVE_H
#include <stdint.h>             // for uintptr_t

const char *sudoku_get_version ();

/// Types of events
typedef enum
{
  ON_INIT = 1,
  ON_CHANGE = 2,
  ON_SOLVED = 4
} sudokuGridEventType;

#ifndef SUDOKU_SIZE
#define SUDOKU_SIZE (3)
#endif

#if SUDOKU_SIZE > 5
#error SUDOKU_SIZE should not exceed 5
#endif

// compile-time integer constant expression
enum
{
  SQUARE_SIZE = SUDOKU_SIZE,
  GRID_SIZE = SQUARE_SIZE * SQUARE_SIZE,
};

typedef struct
{
  char *row_name;
  char *column_name;
  char *value_name;
  char empty_code;
} GridReferential;

extern const GridReferential sudoku_grid_referential;

/// Type definition for events
typedef struct sudoku_grid_event_args
{
  int grid[GRID_SIZE][GRID_SIZE][GRID_SIZE];    ///< Grid of 9 rows, 9 columns and 9 candidate values
  int nbCells;                  ///< Number of non empty cells (for which the possible value has been found).
} sudoku_grid_event_args;

/// Type definition for callback functions called on events.
typedef void (*sudoku_grid_event_handler) (uintptr_t, sudoku_grid_event_args);

/// Adds a callback function called on events.
/// @param [in] event_type Types (or'ed) of event to which the function is to be added.
/// @param [in] handler Function pointer to be added.
void sudoku_grid_event_handler_add (sudokuGridEventType event_type, sudoku_grid_event_handler handler);

/// Removes a callback function called on events.
/// @param [in] type Types (or'ed) of event to which the function is to be removed.
/// @param [in] handler Function pointer to be added.
void sudoku_grid_event_handler_remove (sudokuGridEventType event_type, sudoku_grid_event_handler handler);

/// Type definition for messages
typedef struct sudoku_message_args
{
  const char *rule;             ///< Text of the rule
  int verbosity;                ///< Level of the rule
} sudoku_message_args;

/// Type definition for callback functions called on message.
typedef void (*sudoku_message_handler) (uintptr_t, sudoku_message_args);

/// Adds a callback function called on message.
/// @param [in] handler Function pointer to be added.
void sudoku_message_handler_add (sudoku_message_handler);

/// Removes a callback function called on message.
/// @param [in] handler Function pointer to be added.
void sudoku_message_handler_remove (sudoku_message_handler);

/// Removes all event handlers
void sudoku_all_handlers_clear ();

/// Enumeration of methods avalaible and used for solving
typedef enum
{
  NONE,                         ///< None, when the grid can not be solved.
  EXACT_COVER,                  ///< Exact cover search using dancing links algorithm (brut force)
  ELIMINATION,                  ///< Elimination (human behavior)
  BACKTRACKING,                 ///< Brut force using backtracking
} method;

/// Option to search for the first or all of the possible solutions.
typedef enum
{
  FIRST,                        ///< First only
  ALL                           ///< All of the solutions
} findSolutions;

void sudoku_init (void);

/// Solves the sudoku grid.
/// @param [in] startGrid Grid to be solved
/// @param [in] selected_method Method selected for solving the grid
/// @param [in] option option to choose to search for the first (#FIRST) or all (#ALL) of the possible solutions
/// @returns The method effectively used to solve the grid (promoted to #BACKTRACKING if needed).
method sudoku_solve (int startGrid[GRID_SIZE][GRID_SIZE], method selected_method, findSolutions option);
#endif
