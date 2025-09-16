#include "RangeType.h"

using namespace Dynamix;

RangeType* RangeType::Get() {
	static RangeType type;
	return &type;
}
