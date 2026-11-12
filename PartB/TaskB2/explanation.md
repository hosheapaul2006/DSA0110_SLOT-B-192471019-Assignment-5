# B2 - Deliberate errors, rethrowing and resource safety

## Controlled functional errors

main.cpp uses the unchanged B1 model to exercise negative charges, insurer share above cover, unknown package codes and a missing patient record. It also tests std::stod invalid text and overflow, rejects trailing junk/nonfinite parsed values, and checks valid numeric boundaries. The existing bill remains in the ledger after all these failures; an explicit SelfPay retry succeeds. The conversion routine consumes the full input (leading whitespace accepted by stod, trailing nonnumeric text rejected).

allocation_probe.hpp injects std::bad_alloc at a class-specific operator new before invoking OS allocation. The source/output explicitly say it is injected. No huge allocation, overcommit assumption or memory exhaustion is used. It proves exception handling, no construction of a failed object, and restoration of the injector state; a separate tiny normal allocation verifies ordinary owner cleanup. It does not measure or reproduce real system OOM recovery.

The per-request loop deliberately throws an int, a ClaimError and then processes a successful request. Automatic ScopeTrace and unique_ptr objects are destroyed before each handler/next iteration. The catch-all handles the int and permits the next independent input to run. A separate file-scope exception destroys an ofstream/owner; reopening the file observes the flushed line and removal succeeds after closure. Trace destructors are noexcept and best-effort. All constructed allocation probes are accounted for; actual sanitizer runs provide additional leak checking.

## Supplied prediction and TODO preservation

The immutable prediction was committed in 94ffa2c before execution. Exact actual output matches `ok done caught negative [log too large] caught too large ` plus newline. process logs only the out_of_range case and bare-rethrows; invalid_argument goes directly to main. Neither throwing call prints done.

complete_code.cpp changes only TODO 1-2 in the provided source. approve rejects negative billed/cover values and a gross billed amount above cover; settle logs ClaimError and uses bare throw. The required four-line output is compared exactly. This intentionally minimal exercise does not add the unrelated full production input-validation policy. experiments/throw_copy_exact.cpp changes only throw; to throw e; inside a handler statically typed ClaimError. It prints the same expected result because this example copies ClaimError without changing its dynamic type.

experiments/rethrow_types.cpp observes four cases using a DetailedClaimError with a diagnostic payload and copy identity. Bare rethrow through either an exact or base reference preserves the same detailed object. Throwing an exact DetailedClaimError expression copies it but retains its type/payload. Throwing a ClaimError reference expression copies the base and slices the detailed payload. Counters are reset after the original exception is first caught, so they measure rethrow copies rather than initial throw construction. No address-reuse assumptions are needed.

## One-million-claim optional/exception experiment

benchmark.cpp compares exceptions and optional over exactly 1000000 claims at 0%, 1% and 50% invalid. Valid inputs are 40000 against cover 50000; invalid inputs are 60000. Each rate has an exact invalid count, then a seeded shuffle (192471019+rate). Both methods reuse the same input vector/order and the same finite/nonnegative/cover predicate. These gross-claim experiments match the supplied approve policy, not the application insurance co-payment calculation.

There are five trials per method/rate; method order rotates each trial. steady_clock measures the full validation loop including error construction/throw/catch or optional testing, plus one observable checksum store. It excludes allocation/shuffling, test assertions and output/logging. No console output occurs per claim. Each measured loop verifies the independent rejection count and approved sum. The 8 MB input array is bounded; no one-million exception objects are retained at once. Exceptions are caught immediately per claim. Sources compile with C++17 -Wall -Wextra -Wpedantic -O2.

Raw timings and exact commands/hashes are in outputs/benchmark_O2_*; benchmark_results.md derives five-trial medians from those actual CSV rows. functional_validation.cpp runs a small 1000-claim equivalent plus boundary/nonfinite cases for clear Programiz evidence. It compares optional, exceptions and return codes; large timing claims come only from local optimized execution.

Both schemes take O(n) validation work; optional adds a presence tag/value and local branch, while exceptions incur unwinding/exception-object work on invalid inputs. Capturing a detailed failure reason is an advantage of exceptions; optional alone loses the reason. It is not correct to assume exceptions are always slower at every rate: use the actual measured medians, including the zero-error path. Exception cost depends on runtime, hierarchy, strings, optimization and error distribution. No logging/I/O cost is included in this microbenchmark. sizeof(optional<double>), result wrappers and exception/message allocations are separate storage questions, not inferred from time alone.

## Interpretation of the measured dataset

In the saved five-trial observations, the zero-error medians were similar, while exceptions were substantially slower than optional at both 1% and 50% invalid inputs. That supports using an explicit rejection result for frequent expected rejections in this workload, while preserving exceptions for failures requiring cross-layer propagation. Exact current numbers and all trials are in benchmark_results.md. The small functional driver also prints sizeof optional<double>, StatusResult and ClaimError; sizeof an exception object excludes runtime metadata and any message allocation. No general allocator cost is inferred from that fixed object size.

## Actual online environment limitation

The original full driver failed to create its relative-path temporary file on Programiz. That raw failure is retained in outputs/main_programiz_readonly_failure.txt and screenshots/main_programiz_readonly_failure.jpg. Alternate paths are unverified: an attempted retry reused the original source, and its duplicate capture was discarded. The independent online_main_programiz.cpp runs a labelled 24-check memory-only subset. Original main.cpp and its 25 checks, including actual file flush/close/reopen/remove, remain unchanged and passed locally and on Ubuntu ASan. See [the screenshot index](screenshot_index.md) for the exact submitted sources and the additional ERROR! UI annotation observed in the TODO/copy examples.
