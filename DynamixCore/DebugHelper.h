#pragma once

#include "AstNode.h"

namespace Dynamix {
	class DebugHelper final {
		static const char* AstNodeTypeToString(AstNodeType type) noexcept;
	};
}

