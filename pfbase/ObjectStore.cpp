#include "ObjectStore.h"
#include "MaterialStore.h"
#include "DiagramStore.h"
#include "OtherStore.h"

namespace ProjectBase
{

void Store::MakeStore(FactoryObjectType type, std::function<void(std::shared_ptr<Store>)> func)
{
	switch (type)
	{
	case ObjectTypeMaterialStore:
		func(std::make_shared<MaterialStore>());
		break;
	case ObjectTypeDiagramStore:
		func(std::make_shared<DiagramStore>());
		break;
	case ObjectTypeOtherStore:
		func(std::make_shared<OtherStore>());
		break;
	}

}

} // namespace ProjectBase
