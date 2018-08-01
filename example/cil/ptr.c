/**
 * Copyright (C) 2017 Quenza Inc. All Rights Reserved.
 * Copyright (C) 2018 Blub Corp. All Rights Reserved.
 *
 * This file is part of the Cryptox project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

/**
 * @name        ptr.c
 * @purpose     Demonstration of pointers
 * @author      Blub Corp.
 * @modified    Yorick de Wid
 * @created     2018-04-09
 * @copyright   (C) Copyright 2018, Blub Corp, All Rights Reserved.
 * @license     GPL, Version 3
 */

int main(void) {
    int a, b;
    int *p = &a;
    int *q = &b + 1;

    const int v = p == q;
    // printf("%p %p %d\n", (void *)p, (void *)q, v);
    return 0;
}
