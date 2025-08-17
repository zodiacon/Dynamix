#pragma once

template<unsigned N, typename T = char>
class SmallString {
	static_assert(N > 0);
	static_assert(sizeof(T) == 1 || sizeof(T) == 2);
public:
	SmallString(const T* text = nullptr) {
		if (text) {
			if constexpr (sizeof(T) == 1) {
				strcpy_s(m_Chars, text);
			}
			else {
				wcscpy_s(m_Chars, text);
			}
		}
		else
			m_Chars[0] = (T)0;
	}
	
	operator const T* () const {
		return m_Chars;
	}

private:
	T m_Chars[N + 1];
};

