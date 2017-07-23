#pragma once

#include "File.h"

#include <functional>
#include <memory>
#include <list>

namespace ProjectBase
{

template<class T>
class ObjectStore
{
	std::list<T> nodeList;

protected:
	enum FactoryObjectType {
		ObjectTypeMaterialStore = 1,
		ObjectTypeDiagramStore = 2,
		ObjectTypeOtherStore = 3,
	} m_objectType;

public:
	ObjectStore() = default;

	ObjectStore(FactoryObjectType type)
		: m_objectType{ type }
	{
	}

	void AddNode(const char file[])
	{
		fileList.push_back(T(file));
	}

	void AddNode(T& file)
	{
		fileList.push_back(file);
	}

	virtual ~ObjectStore() {}
};

} // namespace ProjectBase
