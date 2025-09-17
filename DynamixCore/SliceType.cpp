#include "SliceType.h"

using namespace Dynamix;

SliceType* SliceType::Get() {
	static SliceType type;
	return &type;
}

SliceType::SliceType() : ObjectType("Slice") {
}

SliceObject::SliceObject(RuntimeObject* target, Int start, Int count) noexcept
	: RuntimeObject(SliceType::Get()), m_Target(target), m_Start(start), m_Count(count) {
	target->AddRef();
}

SliceObject::~SliceObject() noexcept {
	m_Target->Release();
}
