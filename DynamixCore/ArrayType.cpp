#include "ArrayType.h"
#include "RuntimeObject.h"
#include "Value.h"
#include "Runtime.h"

using namespace Dynamix;

ArrayType& ArrayType::Get() {
	static ArrayType type;
	return type;
}

RuntimeArrayObject* ArrayType::CreateArray(std::vector<Value>& args) {
	return new RuntimeArrayObject(args);
}

Value RuntimeArrayObject::InvokeIndexer(Value const& index) {
	if (!index.IsInteger())
		throw RuntimeError(RuntimeErrorType::TypeMismatch, "Array index must be an integer");
	auto i = index.ToInteger();
	if (i < 0 || i >= (Int)m_Items.size())
		throw RuntimeError(RuntimeErrorType::IndexOutOfRange, std::format("Index {} is out of range (array size: {})", i, m_Items.size()));

	return m_Items[i];
}

ArrayType::ArrayType() : ObjectType("Array") {
	auto size = std::make_unique<MethodInfo>("Size");
	size->Arity = 0;
	size->Flags = MemberFlags::Native;
	size->Code.Native = [](auto, auto& args) -> Value {
		assert(args.size() == 1);
		auto obj = args[0].AsObject();
		return static_cast<Int>(reinterpret_cast<RuntimeArrayObject*>(obj)->Items().size());
		};
	AddMember(std::move(size));

	auto add = std::make_unique<MethodInfo>("Add");
	add->Arity = 1;
	add->Flags = MemberFlags::Native;
	add->Code.Native = [](auto, auto& args) -> Value {
		assert(args.size() == 2);
		auto obj = args[0].AsObject();
		reinterpret_cast<RuntimeArrayObject*>(obj)->Items().push_back(args[1]);
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
