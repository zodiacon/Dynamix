#pragma once

#include <StructObject.h>

namespace SFTypes {
	class Vector2uType : public Dynamix::StructType {
	public:
		static Vector2uType* Get();

	private:
		Vector2uType(std::string name, Dynamix::StructDesc desc);
	};
};

