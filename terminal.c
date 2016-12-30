/**
 * @file
 * Terminal interface for sudoku solver.
 */
/***********************************************************************************
* Author: Laurent Farhi                                                            *
* Name: terminal.c                                                                 *
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

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include "solve.h"
#include "terminal.h"

static display sudokuDisplay;
static int askAgain = 1;

/// Event handler to display the sudoku grid.
/// @param[in] id Grid identifier
/// @param[in] evt_args Event to be processed
static void
grid_print (uintptr_t id, sudoku_grid_event_args evt_args)
{
  static int nbCells;
  int ok[9][9];
  for (int l = 0; l < 9; l++)
    for (int c = 0; c < 9; c++)
    {
      ok[l][c] = 0;
      for (int v = 0; v < 9; v++)
        if (evt_args.grid[l][c][v])
        {
          if (!ok[l][c])
            ok[l][c] = evt_args.grid[l][c][v];
          else
          {
            ok[l][c] = 0;
            break;
          }
        }
    }

  display d = 0;
  if (sudokuDisplay == RULES)
    d = RULES;
  else if (nbCells && (evt_args.nbCells != 81) && (sudokuDisplay & VERBOSE))
    d = VERBOSE;
  else if (!nbCells || (evt_args.nbCells == 81) || ((sudokuDisplay & NORMAL) && (evt_args.nbCells != nbCells)))
    d = NORMAL;

  if (d == VERBOSE)
  {
    printf ("Grid #%u:\n", (unsigned int) id);
    printf ("[%2i]  j   k   l   m   n   o   p   q   r \n", evt_args.nbCells);
    for (int i = 0; i < 81 * 9; i++)
    {
      int l = (i / 27) / 3;
      int c = (i % 27) / 3;
      int v = i % 3 + ((i / 27) % 3) * 3 + 1;

      if (i % (27 * 3 * 3) == 0)
        printf ("    +---+---+---+---+---+---+---+---+---+\n");
      else if (i % (27 * 3) == 0)
        printf ("    +...+...+...+...+...+...+...+...+...+\n");

      if ((i + 2 * 27) % (3 * 3 * 3 * 3) == 0)
        printf ("  %c |", 'A' + i / 81);
      else if (i % (3 * 3 * 3) == 0)
        printf ("    |");

      if (!ok[l][c])
      {
        if (evt_args.grid[l][c][v - 1])
          printf ("%i", evt_args.grid[l][c][v - 1]);
        else
          printf (" ");
      }
      else
      {
        if (v == 5)
          printf ("%i", ok[l][c]);
        else
          printf (" ");
      }

      if ((i + 1) % (9 * 3) == 0)
        printf ("|\n");
      else if ((i + 1) % (3 * 3) == 0)
        printf ("|");
      else if ((i + 1) % 3 == 0)
        printf (":");
    }
    printf ("    +---+---+---+---+---+---+---+---+---+\n");
    printf ("\n");
    askAgain = 1;
  }
  else if (d == NORMAL)
  {
    nbCells = evt_args.nbCells;
    printf ("Grid #%u:\n", (unsigned int) id);
    printf ("[%2i] j k l m n o p q r\n", evt_args.nbCells);
    for (int l = 0; l < 9; l++)
    {
      if (l % 3 == 0)
        printf ("    +-----+-----+-----+\n");
      printf ("  %c |", 'A' + l);
      for (int c = 1; c <= 9; c++)
      {
        int val = 0;
        for (int v = 1; v <= 9; v++)
        {
          if (evt_args.grid[l][c - 1][v - 1])
          {
            if (val)
            {
              v = 10;
              val = 0;
            }
            else
              val = evt_args.grid[l][c - 1][v - 1];
          }
        }
        if (val)
          printf ("%i", val);
        else
          printf (".");
        if (c % 3 == 0)
          printf ("|");
        else
          printf (" ");
      }
      printf ("\n");
    }
    printf ("    +-----+-----+-----+\n");
    printf ("\n");
    askAgain = 1;
  }
  else if (d == RULES)
  {
    nbCells = evt_args.nbCells;
    printf ("Grid #%u: [%2i] ", (unsigned int) id, evt_args.nbCells);
    for (int l = 0; l < 9; l++)
    {
      for (int c = 0; c < 9; c++)
      {
        int val = 0;
        for (int v = 1; v <= 9; v++)
        {
          if (evt_args.grid[l][c][v - 1])
          {
            if (val)
            {
              v = 10;
              val = 0;
            }
            else
              val = evt_args.grid[l][c][v - 1];
          }
        }
        printf ("%i", val);
      }
    }
    printf ("\n");
    askAgain = 1;
  }
}

/// Event handler for interactive mode.
/// @param[in] id Grid identifier
/// @param[in] evt_args Event to be processed
static void
ask (uintptr_t id, sudoku_grid_event_args evt_args)
{
  if (!askAgain)
    return;
  else
    askAgain = 0;

  printf ("====================================================================\n");
  printf ("g[o]/n[ext]/v[erbosity]/r[ules]/q[uit]?[n]");
  while (1 == 1)
  {
    int c = tolower (getchar ());

    if (c == 'g')
    {
      printf ("\nConfirm (y[es]/n[o])?[n]");
      if (tolower (getchar ()) == 'y')
      {
        sudoku_grid_event_handler_remove (ON_CHANGE, ask);
        terminal_unset ();
      }
      printf ("\n");
      return;
    }
    else if (c == 'r')
    {
      printf ("\n");
      if (terminal_display_get () & RULES)
        terminal_display_set (terminal_display_get () & ~RULES);
      else
        terminal_display_set (terminal_display_get () | RULES);
    }
    else if (c == 'v')
    {
      printf ("\n");
      if (terminal_display_get () & NORMAL)
        terminal_display_set ((terminal_display_get () & ~NORMAL) | VERBOSE);
      else if (terminal_display_get () & VERBOSE)
        terminal_display_set (terminal_display_get () & ~(NORMAL | VERBOSE));
      else
        terminal_display_set (terminal_display_get () | NORMAL);
    }
    else if (c == '\n' || c == 'n')
    {
      printf ("\n");
      return;
    }
    else if (c == 'q')
    {
      printf ("\nConfirm (y[es]/n[o])?[n]");
      if (tolower (getchar ()) == 'y')
      {
        printf ("\n");
        exit (0);
      }
      else
      {
        printf ("\n");
        return;
      }
    }
  }
}

/// Message handler to display a message.
/// @param[in] id Grid identifier
/// @param[in] msg_args Message to be processed
static void
print_message (uintptr_t id, sudoku_message_args msg_args)
{
  if ((sudokuDisplay & VERBOSE) || ((sudokuDisplay & RULES) && (msg_args.verbosity <= 2)) || msg_args.verbosity <= 0)
  {
    if (id)
      printf ("Grid #%u:\n", (unsigned int) id);
    printf ("%s", msg_args.rule);
    askAgain = 1;
  }
}

/**************************************************/
/* Terminal manager */
/**************************************************/
static struct termios old_tio;
static int termios_init = 0;

/// User terminal (standard input stream, that is keyboard) initialization.
static void
terminal_init (void)
{
  if (termios_init)
    return;

  signal (SIGTERM, SIG_IGN);
  signal (SIGQUIT, SIG_IGN);
  signal (SIGHUP, SIG_IGN);

  /* unbuffered_input */
  struct termios new_tio;
  /* get the terminal settings for stdin */
  if (tcgetattr (STDIN_FILENO, &old_tio))
    return;

  termios_init = 1;

  /* we want to keep the old setting to restore them a the end */
  new_tio = old_tio;
  /* disable canonical mode (buffered i/o) and local echo */
  new_tio.c_lflag &= (~ICANON & ~ECHO);
  /* set the new settings immediately */
  tcsetattr (STDIN_FILENO, TCSANOW, &new_tio);
}

/// Restore terminal to its initial state.
static void
terminal_end (void)
{
  if (!termios_init)
    return;

  /* restore the former settings */
  tcsetattr (STDIN_FILENO, TCSANOW, &old_tio);
  termios_init = 0;
}

/// Set terminal (standard input and standard output streams, that are keyboard and terminal screen) for interactive mode.
/// @param[in] iflag 1 for interactive mode
void
terminal_set (int iflag)
{
  // set handlers
  sudoku_grid_event_handler_add (ON_INIT | ON_SOLVED, grid_print);
  sudoku_message_handler_add (print_message);
  if (iflag)
  {
    struct termios st;
    if (tcgetattr (STDIN_FILENO, &st) < 0)
      fprintf (stderr, "No standard input available. Interactive mode disabled.\n");
    else if (tcgetattr (STDOUT_FILENO, &st) < 0)
      fprintf (stderr, "No standard output available. Interactive mode disabled.\n");
    else
    {
      terminal_init ();
      sudoku_grid_event_handler_add (ON_CHANGE, ask);
    }
  }
  else
  {
    terminal_end ();
    sudoku_grid_event_handler_remove (ON_CHANGE, ask);
  }
}

/// Reset terminal.
void
terminal_unset (void)
{
  terminal_end ();
}

/// Get display mode.
/// @return Display mode
display
terminal_display_get (void)
{
  return (sudokuDisplay);
}

/// Set display mode.
/// @param[in] Display mode to set.
/// @return Previous display mode.
display
terminal_display_set (display d)
{
  sudoku_message_handler_remove (print_message);
  sudoku_grid_event_handler_remove (ON_CHANGE, grid_print);

  sudoku_message_handler_add (print_message);
  if ((d & NORMAL) || (d & VERBOSE))
    sudoku_grid_event_handler_add (ON_CHANGE, grid_print);

  printf ("Display mode :%s%s%s%s.\n", (d ? "" : " NONE"), (d & NORMAL ? " GRIDS" : ""),
          (d & VERBOSE ? " CANDIDATES" : ""), (d & RULES ? " RULES" : ""));

  display old = sudokuDisplay;
  sudokuDisplay = d;
  return (old);
}
