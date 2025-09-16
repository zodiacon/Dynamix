#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class RangeType : public ObjectType {
	public:
		static RangeType* Get();

	private:
		RangeType() : ObjectType("Range") {}
	};
}

