#ifndef CONTROLLED_ALLOCATION_PROBE_HPP
#define CONTROLLED_ALLOCATION_PROBE_HPP
#include <cstddef>
#include <new>
// Test-only injector. When enabled, OS allocation is deliberately NOT attempted.
class AllocationProbe {
public:
    inline static bool failNext = false;
    inline static int live = 0, constructed = 0, destroyed = 0;
    static void* operator new(std::size_t bytes) {
        if (failNext) { failNext = false; throw std::bad_alloc(); }
        return ::operator new(bytes);
    }
    static void operator delete(void* pointer) noexcept { ::operator delete(pointer); }
    AllocationProbe() noexcept { ++live; ++constructed; }
    ~AllocationProbe() noexcept { --live; ++destroyed; }
};
class FailNextProbeAllocation {
    bool previous_;
public:
    FailNextProbeAllocation() noexcept : previous_(AllocationProbe::failNext) { AllocationProbe::failNext = true; }
    ~FailNextProbeAllocation() noexcept { AllocationProbe::failNext = previous_; }
    FailNextProbeAllocation(const FailNextProbeAllocation&) = delete;
};
#endif
