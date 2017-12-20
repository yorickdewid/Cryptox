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
 * @purpose     Full C99 language feature set to test Parser
 * @author      Blub Corp.
 * @modified    Yorick de Wid
 * @created     2017-08-12
 * @copyright   (C) Copyright 2017, Blub Corp, All Rights Reserved.
 * @license     GPL, Version 3
 */

//> Preprocessor featureset:
//> - Define
//> - If, Else, Endif
//> - Compiler directives

//> C99 featureset:
//> - Function prototype
//> - Function definition
//> - Forward structure, union declaration
//> - Structure, union declaration with identifier
//> - Anonymous dtructure, union declaration
//> - Forward enum declaration

// Preprocessing include
#include <stdio.h>

// Preprocessing definition
#define PI_CONSTANT     3.14159265358979323846

// Forward declaration
enum value;

// Anonymous enum with assignment
enum {
    ITEM1 = 1,
    ITEM,
} _e_item;

typedef struct {
    int *const i;
    volatile int j;
    
    // Anonymous inline union declaration with variable
    union {
        int q;
        short o;
    } v;
} some_t;

// Typedef with multiple qualifiers
typedef const volatile int u8;

// C99 boolean type with implicit integral cast
volatile _Bool hasIOPT = 1;

// Function prototype
static const float func3(const unsigned char);

// Function definition without literal return type
void func() {
    register int seven = 17;
    
    puts("puts");

    // Local scope enum declaration with unary assignment
    enum value2 {
        ITEM_X,
        ITEM_Y = -724,
    };

    // Iterator with expression after statement body
    do {
        seven--;
        printf("seven %d\n", seven);
    } while (seven > 0);

    // Return without type
    return;
}

// Function declaration with return type and multiple arguments
int func2(char c, int i, double d) {
    double e = i + d;
    signed int o = i + d;

    for (int i = 2; i<10; ++i) {
        printf("I is now %d\n", i);
    }

    // Preprocessing compiler directives
    puts(__LINE__);
    puts(__FILE__);

    // Unary expression with literal
    return -1;
}

static const float func3(const unsigned char q) {
    // Varable initializer with explicit cast
    int v = (int)2;

    // Switch statement
    switch (v) {
        case 1:
            break;
        
        // Call expression in label
        case 2:
            func();
            break;
        
        // Case statement with compound statement
        case 3: {
            break;
        }
        
        case 4:
            puts("Combined label");
            break;
        
        // Default statement
        default:
            printf("Default action");
            break;
    }

    // Useless while loop
    unsigned int r = 2;
    while (r > 0) {
        r = 0;
    }

    // Binary return operator with useless parentheses
    return (((  (13.8123 + PI_CONSTANT)  )));
}

// Function definition with variadic arguments
unsigned func4(unsigned short _i, ...) {

    // Integer types
    int u = 0x10; // 16
    int o = 10; // 10
    int j = 007; // 10

    // If statement without compound truth statement
    if (_i == 0) return 0;

    printf("Recursive %u\n", _i);

    // Call recursion function with prefix unary expression
    return func4(--_i);
}

// Preprocessing if statement
#ifdef NOOPT
void funcx() {
    // Empty compound
}
#endif

void func5() {
    int x;
    auto int y;
}

// Application entrypoint
int main(int argc, char *argv[]) {

    // varable declaration and initialization
    int n = 1;
    int y[] = {12, 34, 45};

    // Local scoped compound
    {
        // Call expression with single parameter
        printf("Sometext");
    }
    
    // Expression with single truth statement
    if (n == 1) {
        func();
    }

    // If statement with truth and alt compound
    if (n > 2) {
        func();
    } else {
        func2('t', y[1], 23.12);
    }

    // Decending recursive call expression
    func4(10);
}
