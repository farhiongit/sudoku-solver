/**
 * @file
 * Sudoku solver terminal interface.
 */
/***********************************************************************************
* Author: Laurent Farhi                                                            *
* Name: terminal.h                                                                 *
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
#ifndef TERMINAL_H
#define TERMINAL_H
typedef enum
{
  NORMAL = 1,
  RULES = 2,
  VERBOSE = 4
} display;

void terminal_set (int iflag);
void terminal_unset (void);
display terminal_display_get (void);
display terminal_display_set (display);
#endif
