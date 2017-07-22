// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORE_CONFIG_H_
#define STORE_CONFIG_H_

namespace ProjectBase {

namespace Config {

// Denote version number of the project
constexpr int version = 0x7;

// Project store file banner
constexpr unsigned char BANNER[] = { 'C','R','Y','T','O','X','P','R','O','J','E','C','T','\n','\0','\0' };

} // namespace Config

} // namespace ProjectBase

#endif  // STORE_CONFIG_H_
