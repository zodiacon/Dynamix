#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class RealType : public StaticObjectType {
	public:
		static RealType* Get();

	private:
		RealType();
	};
}

