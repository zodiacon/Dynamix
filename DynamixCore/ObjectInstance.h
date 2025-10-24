#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class ObjectInstance : public StaticObjectType {
	public:
		static ObjectInstance* Get();

	public:
		static Value IsObject(Value const& value) noexcept;
		static Value IsEmpty(Value const& value) noexcept;

	private:
		ObjectInstance();
	};
}
