#include <format>
#include "RangeType.h"

using namespace Dynamix;
using namespace std;

RangeType::RangeType() : ObjectType("Range") {
	struct {
		const char* Name;
		int Arity;
		NativeFunction Code;
		SymbolFlags Flags{ SymbolFlags::Native };
	} methods[] = {
		{ "Size", 0,
			[](auto, auto& args) -> Value {
				assert(args.size() == 1);
				return GetInstance<RangeObject>(args[0])->Size();
				} },
		{ "Shift", 1,
			[](auto, auto& args) -> Value {
				assert(args.size() == 2);
				auto inst = GetInstance<RangeObject>(args[0]);
				inst->Shift(args[1].ToInteger());
				return inst;
				} },
		{ "Start", 1,
			[](auto, auto& args) -> Value {
				assert(args.size() == 2);
				auto inst = GetInstance<RangeObject>(args[0]);
				inst->Start(args[1].ToInteger());
				return inst;
				} },
		{ "Start", 0,
			[](auto, auto& args) -> Value {
				assert(args.size() == 1);
				return GetInstance<RangeObject>(args[0])->Start();
				} },
		{ "End", 1,
			[](auto, auto& args) -> Value {
				assert(args.size() == 2);
				auto inst = GetInstance<RangeObject>(args[0]);
				inst->End(args[1].ToInteger());
				return inst;
				} },
		{ "End", 0,
			[](auto, auto& args) -> Value {
				assert(args.size() == 1);
				return GetInstance<RangeObject>(args[0])->End();
				} },
	};
	for (auto& m : methods) {
		auto mi = std::make_unique<MethodInfo>(m.Name);
		mi->Arity = m.Arity;
		mi->Code.Native = m.Code;
		mi->Flags = m.Flags;
		AddMethod(move(mi));
	}
}

RangeType* RangeType::Get() {
	static RangeType type;
	return &type;
}

RangeObject* RangeType::CreateRange(Int start, Int end) {
	return new RangeObject(start, end);
}

RangeObject::RangeObject(Int start, Int end) : RuntimeObject(RangeType::Get()), m_Start(start), m_End(end) {
}

unique_ptr<IEnumerator> RangeObject::GetEnumerator() const {
	return make_unique<Enumerator>(m_Start, m_End);
}

RuntimeObject* RangeObject::Clone() const {
	return new RangeObject(m_Start, m_End);
}

std::string RangeObject::ToString() const {
	return std::format("{}..{}", m_Start, m_End);
}

RangeObject::Enumerator::Enumerator(Int start, Int end) : m_Current(start), m_End(end) {
}

Value RangeObject::Enumerator::GetNextValue() {
	if (m_Current == m_End)
		return Value::Error(ValueErrorType::CollectionEnd);

	return Value(m_Current++);
}

void* RangeObject::QueryService(ServiceId id) {
	switch (id) {
		case ServiceId::Enumerable: return static_cast<IEnumerable*>(this);
		case ServiceId::Clonable: return static_cast<IClonable*>(this);
	}
	return nullptr;
}
