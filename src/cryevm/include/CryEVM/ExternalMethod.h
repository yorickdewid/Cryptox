// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/AST.h>
#include <CryCC/SubValue.h>

#include <string>
#include <functional>
#include <memory>

// FUTURE:
// - More boilerplating

#define CRY_METHOD(c) void cry_##c(EVM::ExternalFunctionContext& ctx)

#define GET_PARAMETER(i,c) \
	const auto& value##i = ctx->ValueByIdentifier("__arg" #i "__").lock(); \
	assert(value##i); \
	const auto param##i = value##i->As<c>();

#define GET_VA_PARAMETER(i,c) \
	const auto& va_value##i = ctx->ValueByIdentifier("__va_arg" #i "__").lock(); \
	assert(va_value##i); \
	const auto va_param##i = va_value##i->As<c>();

#define SET_RETURN(r) ctx->CreateSpecialVar<RETURN_VALUE>(Util::MakeInt(r));

#define REGISTER_METHOD(s,c) ExternalMethod{ s, &cry_##c }
#define REGISTER_METHOD_PARAM(s,c,...) ExternalMethod{ s, &cry_##c, { __VA_ARGS__ } }

namespace EVM
{

// Function context passed when external method is called.
class ExternalFunctionContext
{
	std::unique_ptr<Valuedef::Value> m_returnValue;
	std::function<std::shared_ptr<Valuedef::Value>(const std::string&)> m_ctxCallback; //FUTURE: Get rid of the ptr?

public:
	// The constructor is initialized with a callback to the context. The context is queryable from
	// this class. The context is the runtime environment in which this method is executed.
	ExternalFunctionContext(std::function<std::shared_ptr<Valuedef::Value>(const std::string&)> cb)
		: m_ctxCallback{ cb }
	{
	}

	auto GetParameter(const std::string& name)
	{
		return m_ctxCallback(name);
	}

	// Set routine return value.
	void SetReturn(Valuedef::Value& value)
	{
		m_returnValue = std::make_unique<Valuedef::Value>(value);
	}

	// Set routine return value.
	void SetReturn(Valuedef::Value&& value)
	{
		m_returnValue = std::make_unique<Valuedef::Value>(std::move(value));
	}

	// Check if a return type was set in this context.
	inline bool HasReturn() const noexcept { return m_returnValue != nullptr; }

	// Get the return, if set.
	std::unique_ptr<Valuedef::Value> GetReturn()
	{
		return std::move(m_returnValue);
	}
};

// Definition of an externally defined method callable by any EVM runner.
class ExternalMethod
{
public:
	class Parameter
	{
	public:
		Parameter(const std::string& identifier, const Typedef::TypeFacade& type)
			: m_identifier{ identifier }
			, m_type{ type }
		{
		}

		explicit Parameter(const std::string& identifier)
			: m_identifier{ identifier }
			, m_isVariadic{ true }
		{
		}

		inline const std::string Identifier() const noexcept { return m_identifier; }
		inline bool Empty() const noexcept { return m_identifier.empty(); }
		inline Typedef::TypeFacade DataType() const noexcept { return m_type; }
		inline bool IsVariadic() const noexcept { return m_isVariadic; }

	private:
		const bool m_isVariadic{ false }; //TODO: incorporate in datatype
		const std::string m_identifier;
		const Typedef::TypeFacade m_type;
	};

	class ParameterList : public std::vector<Parameter>
	{
	public:
		ParameterList() = default;

		ParameterList(std::initializer_list<Parameter>&& list)
			: std::vector<Parameter>{ std::move(list) }
		{
		}

		ParameterList(Parameter&& list)
			: std::vector<Parameter>{ { std::move(list) } }
		{
		}

		// Return the number of parameters.
		size_t Count() const noexcept { return this->size(); }
	};

public:
	using FunctionalType = std::function<void(ExternalFunctionContext&)>;

public:
	ExternalMethod(const std::string& symbol, FunctionalType func, ParameterList params = {})
		: m_symbol{ symbol }
		, m_functional{ func }
		, m_paramList{ params }
	{
	}

	// Get the parameter list.
	const ParameterList& Parameters() const noexcept { return m_paramList; }

	// Get the symbol name.
	std::string Symbol() const noexcept { return m_symbol; }

	// Call the external method.
	void Call(ExternalFunctionContext& ctx) const { m_functional(ctx); }

	// Call the external method via function operator.
	void operator()(ExternalFunctionContext& ctx) const { this->Call(ctx); }

private:
	const std::string m_symbol;
	const FunctionalType m_functional;
	ParameterList m_paramList;
};

} // namespace EVM
