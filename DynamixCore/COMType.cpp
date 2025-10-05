#include "COMType.h"
#include "TypeHelper.h"
#include <Windows.h>
#include "Runtime.h"

using namespace Dynamix;

COMType* COMType::Get() {
	static COMType type;
	return &type;
}

Value COMType::Create(Value const& cls) {
	auto str = cls.ToString();
	CLSID clsid;
	HRESULT hr;
	auto wstr = std::wstring(str.begin(), str.end());
	if (str[0] == '{') {
		hr = ::CLSIDFromString(wstr.c_str(), &clsid);
	}
	else {
		hr = ::CLSIDFromProgID(wstr.c_str(), &clsid);
	}
	if (FAILED(hr))
		return Value::HResult(hr);
	CComPtr<IDispatch> disp;
	hr = disp.CoCreateInstance(clsid);
	if (FAILED(hr))
		return Value::HResult(hr);

	auto obj = new COMObject(disp);
	return static_cast<RuntimeObject*>(obj);
}

COMType::COMType() : StaticObjectType("COM") {
	BEGIN_METHODS(COMType)
		METHOD_STATIC(CreateInstance, 1, return COMType::Create(args[0]);),
		END_METHODS()
}

COMObject::COMObject(IDispatch* disp) : RuntimeObject(COMType::Get()), m_Dispatch(disp) {
}

CComVariant ValueToVariant(Value const& v) {
	switch (v.Type()) {
		case ValueType::Integer:
			return v.AsInteger() < (1LL << 32) ? CComVariant((int)v.AsInteger()) : CComVariant(v.AsInteger());
		case ValueType::Real:
			return CComVariant((double)v.ToReal());
		case ValueType::Object:
			if (v.AsObject()->Type() != COMType::Get())
				throw RuntimeError(RuntimeErrorType::NotCOMObject, "Cannot pass non-COM object to COM object method");
			return CComVariant(reinterpret_cast<COMObject const*>(v.AsObject())->GetDispatch());
		case ValueType::String:
			return CComVariant(CComBSTR(v.ToString().c_str()));
	}
	return CComVariant();
}

Value VariantToValue(CComVariant const& v) {
	switch (v.vt) {
		case VT_DISPATCH:
		case VT_UNKNOWN:
			return new COMObject(v.pdispVal);

		case VT_I2: return Int(v.iVal);
		case VT_UI2: return Int(v.uiVal);
		case VT_I4: return Int(v.intVal);
		case VT_UI4: return Int(v.uintVal);
		case VT_I8: return Int(v.llVal);
		case VT_UI8: return Int(v.ullVal);
		case VT_R4: return Real(v.fltVal);
		case VT_R8: return Real(v.dblVal);
		case VT_BSTR: return new BSTR(v.bstrVal);
	}
	return Value();
}

Value COMObject::Invoke(Interpreter& intr, std::string const& name, std::vector<Value>& args, InvokeFlags flags) {
	DISPID id;
	if (auto hr = GetDispId(name, &id); FAILED(hr))
		return Value::HResult(hr);

	std::vector<CComVariant> v;
	v.reserve(args.size());
	for (auto& arg : args) {
		v.push_back(ValueToVariant(arg));
	}
	CComVariant result;
	auto hr = m_Dispatch.InvokeN(id, v.data(), (int)v.size(), &result);
	if (FAILED(hr))
		return Value::HResult(hr);
	return VariantToValue(result);
}

void COMObject::AssignField(std::string const& name, Value value, TokenType assignType) {
	DISPID id;
	if (FAILED(GetDispId(name, &id)))
		throw RuntimeError(RuntimeErrorType::UnknownMember, std::format("Property '{}' not found on COM object", name));

	auto var = ValueToVariant(value);
	auto hr = m_Dispatch.PutProperty(id, &var);
	if (FAILED(hr))
		throw RuntimeError(RuntimeErrorType::PropertyPut, std::format("Property '{}' failed to set on COM object", name));
}

Value COMObject::GetFieldValue(std::string const& name) const {
	DISPID id;
	if (auto hr = GetDispId(name, &id); FAILED(hr))
		return Value::HResult(hr);

	CComVariant result;
	auto hr = m_Dispatch.GetProperty(id, &result);
	return FAILED(hr) ? Value::HResult(hr) : VariantToValue(result);
}

bool COMObject::HasField(std::string const& name) const noexcept {
	DISPID id;
	return SUCCEEDED(GetDispId(name, &id));
}

HRESULT COMObject::GetDispId(std::string const& name, DISPID* id) const {
	if (auto it = m_DispIds.find(name); it != m_DispIds.end())
		*id = it->second;
	else {
		auto hr = m_Dispatch.GetIDOfName(std::wstring(name.begin(), name.end()).c_str(), id);
		if (FAILED(hr))
			return hr;
		m_DispIds.insert({ name, *id });
	}
	return S_OK;
}
