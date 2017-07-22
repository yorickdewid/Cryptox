// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef PFBASE_H_
#define PFBASE_H_

#if defined(_MSC_VER)
#pragma once
#endif

#ifdef _WIN32
# if defined(PFBASE_EXPORTS)
#  define PFBASEAPI   __declspec(dllexport)
# else
#  define PFBASEAPI   __declspec(dllimport)
# endif
#elif
# define PFBASEAPI
#endif

#include "Meta.h"
#include "Project.h"

#endif // PFBASE_H_
