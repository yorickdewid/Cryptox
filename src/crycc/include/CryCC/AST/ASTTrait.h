// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

namespace CryCC
{
namespace AST
{
namespace Trait
{

// Identifying tag for root nodes.
struct RootNodeTag
{
};

namespace Detail
{

template <typename Node, typename = void>
struct is_root_node : std::false_type {};

template <typename Node>
struct is_root_node<Node, std::enable_if_t<std::is_same<typename Node::NodeTrait, RootNodeTag>::value>> : std::true_type {};

} // namespace detail

template<typename Node>
struct is_root_node : Detail::is_root_node<Node> {};

template<class Node>
constexpr bool is_root_node_v = is_root_node<Node>::value;

} // namespace Trait
} // namespace AST
} // namespace CryCC
