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
 * @name        seqpoint.c
 * @purpose     Test of sequence point evaluation
 * @author      Blub Corp.
 * @modified    Yorick de Wid
 * @created     2018-05-26
 * @copyright   (C) Copyright 2018, Blub Corp, All Rights Reserved.
 * @license     GPL, Version 3
 */

#include <stdio.h>

int func() {
    int u = 9;
    int i = ++u + u;
    return i;
}

int func2() {
    int u = 9;
    int i = u++ + u;
    return i;
}

int main()  {
    func();
    func2();

    return 0;
}
