#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class StringType : public ObjectType {
	public:
		StringType();
	};

	class StringObject : public RuntimeObject, public IEnumerable, public IClonable {
	public:
		StringObject();


	private:
		std::string m_String;
	};
}

