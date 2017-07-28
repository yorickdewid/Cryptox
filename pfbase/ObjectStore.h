#pragma once

#include <functional>
#include <memory>
#include <list>

namespace ProjectBase
{

struct Store
{
	enum FactoryObjectType {
		ObjectTypeMaterialStore = 1,
		ObjectTypeDiagramStore = 2,
		ObjectTypeOtherStore = 3,
	} m_objectType;

	Store(FactoryObjectType type)
		: m_objectType{ type }
	{
	}

	FactoryObjectType Type() const
	{
		return m_objectType;
	}

	static void MakeStore(FactoryObjectType type, const std::string& content, std::function<void(std::shared_ptr<Store>)> func);

	// Force the subclass to implement a print out function. This should only be used
	// to return class inner data which needs to be stored.
	virtual void Print(std::ostream& out) const = 0;
	virtual void Parse(const std::string content) = 0;

	friend std::ostream& operator<<(std::ostream& out, const Store& c)
	{
		c.Print(out);
		return out;
	}

	virtual ~Store() {}
};

template<class T>
class ObjectStore : public Store
{
protected:
	std::list<T> nodeList;

public:
	ObjectStore() = default;

	ObjectStore(FactoryObjectType type)
		: Store{ type }
	{
	}

	void AddNode(const char file[])
	{
		nodeList.push_back(T(file));
	}

	void AddNode(T& file)
	{
		nodeList.push_back(file);
	}

	virtual ~ObjectStore() {}
};

} // namespace ProjectBase
