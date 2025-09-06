#pragma once

namespace Dynamix {
	struct NoCopy {
		NoCopy() = default;
	private:
		NoCopy(NoCopy const&) = delete;
		NoCopy& operator=(NoCopy const&) = delete;
	};

	struct NoMove {
		NoMove() = default;
	private:
		NoMove(NoMove&&) = delete;
		NoMove& operator=(NoMove&&) = delete;
	};
}
