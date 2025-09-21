#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class StringType : public StaticObjectType {
	public:
		static StringType* Get();

	private:
		StringType();
	};

	template<typename Char>
	class StringObject : public RuntimeObject, public IEnumerable, public IClonable {
	public:
		using String = std::basic_string<Char>;

		StringObject(const Char* s = nullptr) : m_String(s) {}
		StringObject(String s) : m_String(std::move(s)) {}

		int64_t Length() const noexcept {
			return m_String.length();
		}

		StringObject* SubString(int64_t start, int64_t length = -1) {
			return new StringObject(m_String.substr(start, length));
		}

		SliceObject* Slice(int64_t start, int64_t length = -1);

	private:
		String m_String;
	};

	using StringObjectA = StringObject<char>;
	using StringObjectW = StringObject<wchar_t>;
}

