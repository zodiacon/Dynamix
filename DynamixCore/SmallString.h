#pragma once

#include <string>

template<unsigned N, typename T = char>
class SmallString {
	static_assert(N > 0);
	static_assert(sizeof(T) == 1 || sizeof(T) == 2);
public:
	SmallString(const T* text = nullptr) {
		if (text) {
			if constexpr (sizeof(T) == 1) {
				if ((m_Count = strlen(text)) > N) {
					m_pChars = new T[m_Count + 1];
					strcpy_s(m_pChars, m_Count + 1, text);
				}
				else {
					strcpy_s(m_Chars, text);
				}
			}
			else {
				wcscpy_s(m_Chars, text);
			}
		}
		else {
			m_Count = 0;
			m_Chars[0] = 0;
		}
	}

	SmallString(SmallString const& other) {
		if (other.m_Count > N) {
			m_pChars = new T[other.m_Count + 1];
			strcpy_s(m_pChars, other.m_Count + 1, other.m_pChars);
		}
		else {
			strcpy_s(m_Chars, other.m_Chars);
		}
		m_Count = other.m_Count;
	}

	SmallString& operator=(SmallString const& other) {
		if (this != &other) {
			Free();
			if (other.m_Count > N) {
				m_pChars = new T[other.m_Count + 1];
				strcpy_s(m_pChars, other.m_Count + 1, other.m_pChars);
			}
			else {
				strcpy_s(m_Chars, other.m_Chars);
			}
			m_Count = other.m_Count;
		}
		return *this;
	}

	SmallString(SmallString&& other) noexcept {
		m_Count = other.m_Count;
		if (other.m_Count > N) {
			m_pChars = other.m_pChars;
			other.m_pChars = nullptr;
		}
		else {
			strcpy_s(m_Chars, other.m_Chars);
		}
		other.m_Count = 0;
	}

	SmallString& operator=(SmallString&& other) noexcept {
		if (this != &other) {
			Free();
			m_Count = other.m_Count;
			if (other.m_Count > N) {
				m_pChars = other.m_pChars;
				other.m_pChars = nullptr;
			}
			else {
				strcpy_s(m_Chars, other.m_Chars);
			}
			other.m_Count = 0;
		}
		return *this;
	}

	~SmallString() noexcept {
		Free();
	}

	SmallString& operator +=(const T* text) {
		if (text) {
			auto len = strlen(text);
			if (m_Count + len > N) {
				if (m_Count <= N) {
					auto pNew = new T[m_Count + len + 1];
					strcpy_s(pNew, m_Count + 1, m_Chars);
					strcpy_s(pNew + m_Count, len + 1, text);
					m_pChars = pNew;
				}
				else {
					auto pNew = new T[m_Count + len + 1];
					strcpy_s(pNew, m_Count + 1, m_pChars);
					strcpy_s(pNew + m_Count, len + 1, text);
					delete[] m_pChars;
					m_pChars = pNew;
				}
			}
			else {
				strcpy_s(m_Chars + m_Count, len + 1, text);
			}
			m_Count += len;
		}
		return *this;
	}

	void Free() noexcept {
		if (m_Count > N) {
			delete[] m_pChars;
			m_pChars = nullptr;
		}
		else {
			m_Chars[0] = 0;
		}
		m_Count = 0;
	}
	const T* c_str() const noexcept {
		return m_Count > N ? m_pChars : m_Chars;
	}

	operator const T* () const {
		return c_str();
	}

	std::basic_string<T> ToStdString() const {
		return c_str();
	}

	bool IsSmall() const noexcept {
		return m_Count <= N;
	}

	unsigned Length() const noexcept {
		return m_Count;
	}

private:
	union {
		T m_Chars[N + 1];
		T* m_pChars;
	};
	unsigned m_Count;
};

