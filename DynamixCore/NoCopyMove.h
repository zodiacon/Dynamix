#pragma once

namespace Dynamix {
	struct NoCopy {
		NoCopy() = default;
		NoCopy(NoCopy&&) = default;
		NoCopy& operator=(NoCopy&&) = default;
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
