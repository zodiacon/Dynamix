#include "RuntimeObject.h"
#include "ObjectType.h"
#include "Value.h"

using namespace Dynamix;

RuntimeObject::RuntimeObject(ObjectType& type) : m_Type(type) {
}

Value RuntimeObject::Invoke(std::string_view name, Value* args, int count) {
	return m_Type.Invoke(this, name, args, count);
}

int RuntimeObject::AddRef() {
	return ++m_RefCount;
}

int RuntimeObject::Release() {
	auto count = --m_RefCount;
	if (count == 0)
		m_Type.DestroyObject(this);
	return 0;
}

