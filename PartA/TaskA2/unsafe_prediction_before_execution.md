# A2 destructor variations predicted before compilation/execution

1. Removing only virtual from ~Bill leaves ~OPBill() override invalid. Predict a compile error because that destructor no longer overrides a virtual destructor. This source must not be executed.
2. For a runnable controlled experiment, also remove override from ~OPBill. Deleting OPBill through Bill* is now undefined behavior: no exact output or sanitizer diagnosis is guaranteed by C++. A common implementation outcome is `500 ~Bill \n` with the derived destructor skipped. Because both fieldless objects can have equal sizes, AddressSanitizer may produce no diagnostic. Such an observation must be labelled UB with no diagnostic observed, never a clean/correct program.

Both modified sources preserve the remaining supplied source. The valid original is separate. These predictions must not be rewritten after observing results.
