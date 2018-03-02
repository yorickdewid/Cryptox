/**
* Copyright (C) 2018 Cryptox, Blub Corp.
* All Rights Reserved
*
* This file is part of the Cryptox project.
*
* Content can not be copied and/or distributed without the express
* permission of the author.
*/

#pragma once

#define _EXTERN extern

#ifdef __cplusplus
extern "C" {
#endif

#define _CRY_STDIO_DEFINED 1

/*
* Output operations.
*/

/* 	Prints an error message to stderr. */
_EXTERN void perror(const char *str);

/* Formats and prints characters and values to stdout. */
_EXTERN int printf(const char *fmt, ...);

/* Prints c to the output stream. */
//_EXTERN int putc(int c, FILE *stream);

/* Prints c to stdout. */
_EXTERN int putchar(int c);

/* Write a string, followed by a newline, to stdout. */
_EXTERN int puts(const char *str);

/*
 * File operations.
 */

/* Deletes the file specified by filename. */
_EXTERN int remove(const char *file);

/* Renames the specified file. */
_EXTERN int rename(const char *oldname, const char *newname);

/* Repositions the file pointer associated with stream to the beginning of the file. */
//_EXTERN void rewind(FILE *stream);

#ifdef __cplusplus
}
#endif
