// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#define	_STDLIB_H

#if (defined __CRYC__ || defined __INCLUDE_CRYLIB) && !defined _CRYLIB_H
# include <crylib.h>
#endif

//
// Program termination.
//

// Abort execution and generate a core-dump.
extern void abort(void);

// Register a function to be called when 'exit' is called.
extern int atexit(void(*__func) (void));

#ifdef __USE_ISOC11
// Register a function to be called when 'quick_exit' is called.
extern int at_quick_exit(void(*__func) (void));
#endif

#ifdef __USE_MISC
// Register a function to be called with the status given
// to 'exit' and the given argument.
extern int on_exit(void(*__func) (int __status, void *__arg), void *__arg);
#endif

// Call all functions registered with 'atexit' and 'on_exit',
// in the reverse of the order in which they were registered then
// perform stdio cleanup, and terminate program execution with STATUS.
extern void exit(int __status);

#ifdef __USE_ISOC11
// Call all functions registered with 'at_quick_exit' in the reverse
// of the order in which they were registered and terminate program
// execution with STATUS.
extern void quick_exit(int __status);
#endif

#ifdef __USE_CRYC
// Call all functions registered with 'at_quick_exit' in the reverse
// of the order in which they were registered and terminate program
// with calltrace.
extern void calltrace_exit(void);
#endif

#ifdef __USE_ISOC99
// Terminate the program with STATUS without calling any of the
// functions registered with 'atexit' or 'on_exit'.
extern void _Exit(int __status);
#endif

// We define these the same for all machines and environments.
#define	EXIT_FAILURE 1 // Failing exit status.
#define	EXIT_SUCCESS 0 // Successful exit status.

//
// Communicating with the environment.
//

// Execute the given line as a shell command.
extern int system(const char *__command);

//
// Signals.
//

//
// Cross jumps.
//
