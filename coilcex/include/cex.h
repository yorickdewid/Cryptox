// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#ifndef COILCEX_H_
#define COILCEX_H_

#if defined(_MSC_VER)
#pragma once
#endif

#ifdef _WIN32
# if defined(COILCEX_EXPORTS) || defined(CoilCEX_EXPORTS)
#  define COILCEXAPI   __declspec(dllexport)
# else
#  define COILCEXAPI   __declspec(dllimport)
# endif
#else
# define COILCEXAPI
#endif

#define COILCEXAPIVER	090

namespace CryExe
{

class COILCEXAPI Executable;

} // namespace CryExe

#endif // COILCEX_H_
