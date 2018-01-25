// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef CONFIG_H_
#define CONFIG_H_

#if defined(_MSC_VER)
#pragma once
#endif

#if defined(PFBASE_EXPORTS)
# define PFBASEAPI   __declspec(dllexport)
#else
# define PFBASEAPI   __declspec(dllimport)
#endif

#endif // CONFIG_H_
