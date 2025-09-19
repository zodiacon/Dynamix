#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class EnumType : public StaticObjectType {
	public:
		static EnumType* Get();

	private:
		EnumType();
	};
}

