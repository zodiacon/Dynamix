#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class IntegerType : public StaticObjectType {
	public:
		static IntegerType* Get();

	private:
		IntegerType();
	};
}

