#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class MathType : public StaticObjectType {
	public:
		static MathType* Get();

	private:
		MathType();
	};
}

