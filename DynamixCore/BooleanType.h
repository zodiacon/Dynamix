#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class BooleanType : public StaticObjectType {
	public:
		static BooleanType* Get();

	private:
		BooleanType();
	};
}

