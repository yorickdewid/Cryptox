#include "ObjectStore.h"
#include "MaterialStore.h"
#include "DiagramStore.h"
#include "OtherStore.h"

namespace ProjectBase
{

void Store::MakeStore(FactoryObjectType type, const std::string& content, std::function<void(std::shared_ptr<Store>)> func)
{
	std::shared_ptr<Store> storeptr;

	switch (type)
	{
	case ObjectTypeMaterialStore:
		storeptr = std::make_shared<MaterialStore>();
		break;
	case ObjectTypeDiagramStore:
		storeptr = std::make_shared<DiagramStore>();
		break;
	case ObjectTypeOtherStore:
		storeptr = std::make_shared<OtherStore>();
		break;
	}

	storeptr->Parse(content);
	func(storeptr);

}

} // namespace ProjectBase
