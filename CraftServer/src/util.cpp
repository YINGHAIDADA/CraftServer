#include "cspch.h"
#include "util.h"

namespace CraftServer {
	pid_t GetThreadId() {
		return syscall(SYS_gettid);
	}

	uint32_t GetFiberId() {
		return 0;
	}

}