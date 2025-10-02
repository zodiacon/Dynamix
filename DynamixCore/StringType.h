#pragma once

#include <string>
#include <memory>
#include "ObjectType.h"
#include "CoreInterfaces.h"
#include "TypeHelper.h"
#include "SliceType.h"
#include "VectorEnumerator.h"

#define STRING_TYPE(Suffix, Char)	\
	class StringObject##Suffix;	\
	class StringType##Suffix : public StaticObjectType {	\
	public:	\
		static StringType##Suffix* Get() {	\
			static StringType##Suffix type;	\
			return &type;	\
		}	\
	private:	\
		StringType##Suffix() : StaticObjectType("String" #Suffix) {	\
			BEGIN_METHODS(StringObject##Suffix)	\
				METHOD(Length, 0, return inst->Length();),	\
				END_METHODS()	\
		}	\
	};	\
\
	class StringObject##Suffix : public RuntimeObject, public IClonable, public ISliceable, public IEnumerable {	\
	public:	\
		using Type = StringObject##Suffix;	\
		using String = std::basic_string<Char>;	\
		StringObject##Suffix(const Char* s = nullptr) : RuntimeObject(StringType##Suffix::Get()), m_String(s) {}	\
		StringObject##Suffix(String s) : RuntimeObject(StringType##Suffix::Get()), m_String(std::move(s)) {}	\
		int64_t Length() const noexcept {	\
			return m_String.length();	\
		}	\
		std::unique_ptr<IEnumerator> GetEnumerator() const override {	\
			return std::make_unique<VectorEnumerator<String::const_iterator>>(m_String.begin(), m_String.end());	\
		}	\
		Type* SubString(int64_t start, int64_t length = -1) const {	\
			return new Type(m_String.substr(start, length));	\
		}	\
		SliceObject* Slice(Int start, Int length = -1) {	\
			return new SliceObject(this, start, length < 0 ? Length() - start : length);	\
		}	\
		Type* Clone() const { return new StringObject##Suffix(m_String); }	\
		void* QueryService(ServiceId id) override {	\
			switch (id) {	\
				case ServiceId::Enumerable: return static_cast<IEnumerable*>(this);	\
				case ServiceId::Clonable: return static_cast<IClonable*>(this);	\
				case ServiceId::Sliceable: return static_cast<ISliceable*>(this);	\
			}	\
			return nullptr; \
		}	\
	private:	\
		String m_String;	\
	}

namespace Dynamix {
	STRING_TYPE(A, char);
	STRING_TYPE(W,wchar_t);

}

