#include "ObjectType.h"
#include "RuntimeObject.h"
#include "Interfaces.h"

using namespace Dynamix;

ObjectType::ObjectType(Runtime& rt) : m_Runtime(rt) {
}

RuntimeObject* ObjectType::CreateObject(Value* args, int count) {
	m_ObjectCount++;
	auto obj = (RuntimeObject*)m_Runtime.GetAllocator()->Allocate(sizeof(RuntimeObject));
	new (obj) RuntimeObject(*this);
	if (count > 0)
		obj->Construct(args, count);
	return obj;
}

void ObjectType::DestroyObject(RuntimeObject* instance) {
	instance->Destruct();
	instance->~RuntimeObject();
	m_Runtime.GetAllocator()->Free(instance);
	--m_ObjectCount;
}

unsigned Dynamix::ObjectType::GetObjectCount() const {
	return m_ObjectCount.load();
}
