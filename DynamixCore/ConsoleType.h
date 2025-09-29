#pragma once

#include <vector>
#include "ObjectType.h"

namespace Dynamix {
	class ConsoleType : StaticObjectType {
	public:
		static ConsoleType* Get();

		static Value Write(std::vector<Value> const& args);
		static Value WriteLine(std::vector<Value> const& args);
		static Value Error(std::vector<Value> const& args);
		static Value ErrorLine(std::vector<Value> const& args);
		static Value ReadLine();

	private:
		ConsoleType();
	};
}

