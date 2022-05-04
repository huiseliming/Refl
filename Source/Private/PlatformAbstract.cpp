#include "PlatformAbstract.h"

#ifdef _WIN32
#include <intrin.h>
#endif

void* IPlatformAbstract::InterlockedCompareExchangePointerImpl(void* volatile* Dest, void* Exchange, void* Comparand)
{
	return ::_InterlockedCompareExchangePointer(Dest, Exchange, Comparand);
}

void* IPlatformAbstract::InterlockedExchangePointerImpl(void** Dest, void* Exchange) {
#ifdef _WIN32
	return ::_InterlockedExchangePointer(Dest, Exchange);
#elif __clang__
	return __sync_lock_test_and_set(Dest, Exchange);
#endif
}

void IPlatformAbstract::MemoryBarrierImpl() {
#ifdef _WIN32
	_mm_sfence();
#elif __clang__
	__sync_synchronize();
#endif
}
unsigned char IPlatformAbstract::BitScanForwardImpl(unsigned long* Index, unsigned long Mask)
{
#ifdef _WIN32
	return _BitScanForward(Index, Mask);
#elif __clang__
	return __builtin_clz(Index, Mask);
#endif
}
unsigned char IPlatformAbstract::BitScanReverseImpl(unsigned long* Index, unsigned long Mask)
{
#ifdef _WIN32
	return _BitScanReverse(Index, Mask);
#elif __clang__
	return __builtin_clz(Index, Mask);
#endif
}
