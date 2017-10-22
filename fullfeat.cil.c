/**
 * Copyright (C) 2017 Cryptox, Blub Corp.
 * All Rights Reserved
 *
 * This file is part of the Cryptox project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

/**
 * @name        fullfeat.cil.c
 * @purpose     Full C99 language feature set
 * @author      Blub Corp.
 * @modified    Yorick de Wid
 * @created     2017-08-12
 * @copyright   (C) Copyright 2017, Blub Corp, All Rights Reserved.
 * @license     GPL, Version 3
 */

#include <stdio.h>

#define PI_CONSTANT     3.14159265358979323846

enum value;

enum {
    ITEM1 = 1,
    ITEM,
};

typedef struct {
    int i;
    int j;
    union {
        int q;
        short o;
    } v;
} some_t;

volatile _Bool hasIOPT = 1;

static const float func3(const unsigned char);

void func() {
    register int seven = 17;
    
    puts("puts");

    enum value2 {
        ITEM_X,
        ITEM_Y,
    };

    do {
        seven--;
        printf("seven %d\n", seven);
    } while (seven > 0);

    return;
}

int func2(char c, int i, double d) {
    double e = i + d;
    signed int o = i + d;

    for (int i = 2; i<10; ++i) {
        printf("I is now %d\n", i);
    }

    return -1;
}

static const float func3(const unsigned char q) {
    int v = (int)2;

    switch (v) {
        case 1: // static int case
            break;
        case 2: // call func
            func();
            break;
        case 3:
        case 4:
            puts("Combined label");
            break;
        default: // default action
            printf("Default action");
            break;
    }

    // Useless while loop
    unsigned int r = 2;
    while (r > 0) {
        r = 0;
    }

    return (13.8123 + PI_CONSTANT);
}

unsigned func4(unsigned short _i, ...) {
    if (_i == 0) return 0;

    printf("Recursive %u\n", _i);

    return func4(--_i);
}

#ifdef NOOPT
void funcx() {
    // empty
}
#endif

/**
 * Application entrypoint
 */
int main(int argc, char *argv[]) {
    int n = 1;
    int y[] = {12, 34, 45};

    printf("Sometext");
    
    // Simple expression
    if (n == 1) {
        func();
    }

    /** Simple selection */
    if (n > 2) {
        func();
    } else {
        func2('t', y[1], 23.12);
    }

    /**
     * Decending recursive function
     */
    func4(10);
}
