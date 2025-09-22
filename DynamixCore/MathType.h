#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class MathType : StaticObjectType {
	public:
		static MathType* Get();

	private:
		MathType();
	};
}

