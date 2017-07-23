#include "ObjectStore.h"
#include "MaterialStore.h"
#include "DiagramStore.h"


//void ObjectStore::MakeStore(std::shared_ptr<ObjectStore> store, std::function<void(const std::string&, std::shared_ptr<ObjectStore>)> func)
//{
//	switch (store->m_objectType)
//	{
//	case ObjectTypeMaterialStore:
//	{
//		auto e = std::dynamic_pointer_cast<MaterialStore>(store);
//		func("kaas", e);
//		break;
//	}
//	case ObjectTypeDiagramStore:
//	{
//		break;
//	}
//	}
//
//}
