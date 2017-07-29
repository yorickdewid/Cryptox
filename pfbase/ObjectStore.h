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

protected:
	void AddNode(const char node[])
	{
		nodeList.push_back(T(node));
	}

	void AddNode(T& node)
	{
		nodeList.push_back(node);
	}

	T& GetNode(const char node[])
	{
		auto it = std::find_if(nodeList.begin(), nodeList.end(), [&node](T& s) {
			return s.Name() == std::string{ node };
		});

		if (it == nodeList.end()) {
			std::runtime_error{ "no file found" };
		}

		return *it;
	}

public:
	size_t Size() const
	{
		return nodeList.size();
	}

	virtual ~ObjectStore() {}
};

} // namespace ProjectBase
