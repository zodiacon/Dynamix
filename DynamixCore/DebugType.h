#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class DebugType : public StaticObjectType {
	public:
		static DebugType* Get();

		static bool Assert(Value const& expr);
		static void Break();

	private:
		DebugType();
	};
}


