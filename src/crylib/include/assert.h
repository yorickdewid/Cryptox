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

#ifdef _DEBUG
# define assert(e) (void)((e) || (__assert(#e, __FILE__, __LINE__),0))
#else
# define assert(e)
#endif

extern void __cryinvoke __assert(const char *msg, const char *file, int line);
#endif
