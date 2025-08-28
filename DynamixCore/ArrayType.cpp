#include "ArrayType.h"
#include "RuntimeObject.h"
#include "Value.h"

using namespace Dynamix;

ArrayType& ArrayType::Get() {
	static ArrayType type;
	return type;
}

RuntimeArrayObject* ArrayType::CreateArray(std::vector<Value>& args) {
	return new RuntimeArrayObject(args);
}

ArrayType::ArrayType() : ObjectType("Array") {
	auto size = std::make_unique<MethodInfo>("Size");
	size->Arity = 0;
	size->Code.Native = [](auto obj, auto&) -> Value {
		return static_cast<Int>(reinterpret_cast<RuntimeArrayObject*>(obj)->Items().size());
		};
	AddMember(std::move(size));

	auto add = std::make_unique<MethodInfo>("Add");
	add->Arity = 1;
	add->Code.Native = [](auto obj, auto& values) -> Value {
		assert(values.size() == 1);
		reinterpret_cast<RuntimeArrayObject*>(obj)->Items().push_back(values[0]);
		return Value(obj);
		};
	AddMember(std::move(add));
}

RuntimeArrayObject::RuntimeArrayObject(std::vector<Value> init) : 
	RuntimeObject(ArrayType::Get()), m_Items(std::move(init)) {
}

std::string RuntimeArrayObject::ToString() const {
	std::string text("[ ");
	for (auto& item : m_Items)
		text += item.ToString() + ", ";
	if (!m_Items.empty())
		text = text.substr(0, text.length() - 2);
	return text + " ]";
}
