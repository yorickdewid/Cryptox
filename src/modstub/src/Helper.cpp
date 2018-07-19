// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

//#include "plgex.h"

#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT

//namespace ExternalModule
//{
//
//class my_plugin_sum : public ModuleInterface {
//public:
//	my_plugin_sum()
//	{
//		//std::cout << "Constructing my_plugin_sum" << std::endl;
//	}
//
//	Info GetInfo() const
//	{
//		return Info{ 12, "kaas" };
//	}
//
//	~my_plugin_sum()
//	{
//		//std::cout << "Destructing my_plugin_sum ;o)" << std::endl;
//	}
//};
//
//// Exporting `my_namespace::plugin` variable with alias name `plugin`
//// (Has the same effect as `BOOST_DLL_ALIAS(my_namespace::plugin, plugin)`)
//extern "C" BOOST_SYMBOL_EXPORT my_plugin_sum exmod;
//my_plugin_sum plugin;
//
//} // namespace ExternalModule
