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

Value COMObject::Invoke(Interpreter& intr, std::string const& name, std::vector<Value>& args, InvokeFlags flags) {
    DISPID id;
    HRESULT hr;
    if (auto it = m_DispIds.find(name); it != m_DispIds.end())
        id = it->second;
    else {
        hr = m_Dispatch.GetIDOfName(std::wstring(name.begin(), name.end()).c_str(), &id);
        if (FAILED(hr))
            return Value::HResult(hr);
        m_DispIds.insert({ name, id });
    }
    std::vector<CComVariant> v;
    v.reserve(args.size());
    for (auto& arg : args) {
        v.push_back(ValueToVariant(arg));
    }
    CComVariant result;
    hr = m_Dispatch.InvokeN(id, v.data(), (int)v.size(), &result);
    if (FAILED(hr))
        return Value::HResult(hr);
    return Value();// VariantToValue(result);
}
