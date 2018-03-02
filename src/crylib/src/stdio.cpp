// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <stdio.h>

#define WRAP_NATIVE(n) cry_##n

int WRAP_NATIVE(puts)(const char *str)
{
	return puts(str);
}

void WRAP_NATIVE(perror)(const char *str)
{
	perror(str);
}

int WRAP_NATIVE(remove)(const char *file)
{
	return remove(file);
}
