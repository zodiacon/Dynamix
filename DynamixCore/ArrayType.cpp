#include "ArrayType.h"
#include "RuntimeObject.h"
#include "Runtime.h"

using namespace Dynamix;

ArrayType& ArrayType::Get() {
	static ArrayType type;
	return type;
}

ArrayObject* ArrayType::CreateArray(std::vector<Value>& args) {
	return new ArrayObject(args);
}

Value ArrayObject::InvokeIndexer(Value const& index) {
	if (!index.IsInteger())
		throw RuntimeError(RuntimeErrorType::TypeMismatch, "Array index must be an integer");
	auto i = index.ToInteger();
	if (i < 0 || i >= (Int)m_Items.size())
		throw RuntimeError(RuntimeErrorType::IndexOutOfRange, std::format("Index {} is out of range (array size: {})", i, m_Items.size()));

	return m_Items[i];
}

void ArrayObject::AssignIndexer(Value const& index, Value const& value, TokenType assign) {
	if (!index.IsInteger())
		throw RuntimeError(RuntimeErrorType::TypeMismatch, "Array index must be an integer");
	auto i = index.ToInteger();
	if(i < 0 || i >= (Int)m_Items.size())
		throw RuntimeError(RuntimeErrorType::IndexOutOfRange, std::format("Index {} is out of range (array size: {})", i, m_Items.size()));
	m_Items[i].Assign(value, assign);
}

ArrayType::ArrayType() : ObjectType("Array") {
	auto size = std::make_unique<MethodInfo>("Size");
	size->Arity = 0;
	size->Flags = MemberFlags::Native;
	size->Code.Native = [](auto, auto& args) -> Value {
		assert(args.size() == 1);
		auto obj = args[0].AsObject();
		return static_cast<Int>(reinterpret_cast<ArrayObject*>(obj)->Items().size());
		};
	AddMember(std::move(size));

	auto add = std::make_unique<MethodInfo>("Add");
	add->Arity = 1;
	add->Flags = MemberFlags::Native;
	add->Code.Native = [](auto, auto& args) -> Value {
		assert(args.size() == 2);
		auto obj = args[0].AsObject();
		reinterpret_cast<ArrayObject*>(obj)->Items().push_back(args[1]);
		return Value(obj);
		};
	AddMember(std::move(add));
}

ArrayObject::ArrayObject(std::vector<Value> init) : 
	RuntimeObject(ArrayType::Get()), m_Items(std::move(init)) {
}

std::string ArrayObject::ToString() const {
	std::string text("[ ");
	for (auto& item : m_Items)
		text += item.ToString() + ", ";
	if (!m_Items.empty())
		text = text.substr(0, text.length() - 2);
	return text + " ]";
}
