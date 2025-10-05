#pragma once

#include "ObjectType.h"
#include <atlcomcli.h>
#include <map>
#include <string>

namespace Dynamix {
	class COMObject;

	class COMType : public StaticObjectType {
	public:
		static COMType* Get();

		static Value Create(Value const& cls);

	private:
		COMType();
	};

	class COMObject : public RuntimeObject {
	public:
		explicit COMObject(IDispatch* disp);

		bool SkipCheckNames() const noexcept override {
			return true;
		}

		Value Invoke(Interpreter& intr, std::string const& name, std::vector<Value>& args, InvokeFlags flags) override;

		IDispatch* GetDispatch() const {
			return m_Dispatch.p;
		}

	private:
		struct CaseInsensitiveMatch {
			bool operator()(std::string const& s1, std::string const& s2) const noexcept {
				return _stricmp(s1.c_str(), s2.c_str()) < 0;
			}
		};
		CComPtr<IDispatch> m_Dispatch;
		std::map<std::string, DISPID, CaseInsensitiveMatch> m_DispIds;
	};
}
