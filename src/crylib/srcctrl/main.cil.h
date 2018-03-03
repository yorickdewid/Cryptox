/**
 * Copyright (C) 2017 Cryptox, Blub Corp.
 * All Rights Reserved
 *
 * This file is part of the Cryptox project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#pragma once

struct info {
    int version;
    int counter;
};

/* Formats and prints characters and values to stdout. */
int printf(const char *fmt, ...);

/* Prints a string to stdout. */
int puts(const char *str);
