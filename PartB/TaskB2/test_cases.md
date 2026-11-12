# B2 executed functional and benchmark checks

| ID | Input/scenario | Expected | Actual | Status |
|---|---|---|---|---|
| 1 | Exact supplied validate/process program | ok/done; negative caught directly; too-large logged then rethrown; exact whitespace | Exact prediction matched | PASS |
| 2 | Supplied approve/settle TODO | approved40000; audit/rejected exceeds cover; negative amount error | Exact four-line output matched | PASS |
| 3 | Same TODO with throw e in ClaimError handler | Same dynamic ClaimError type and text, copied object | Exact same output matched | PASS |
| 4 | Negative OP charge | ValidationError and no usable invalid object | Observed message and cleanup checked | PASS |
| 5 | Bill100000, insurer share80000, cover50000 | ClaimError and retained bill | Audited/rethrown; bill pointer/size/total unchanged | PASS |
| 6 | Unknown package and patient ID999 | PackageError / MissingPatientError | Real messages captured | PASS |
| 7 | std::stod abc and 1e99999 | invalid_argument and out_of_range respectively | Both exact exception types checked | PASS |
| 8 | Trailing junk/nan; valid12.50/0 | Reject invalid, accept valid normalized values | Checked | PASS |
| 9 | Scoped fail-next AllocationProbe | Injected bad_alloc before constructor, zero OS allocation attempt | Catch observed; constructor count unchanged; flag restored | PASS |
| 10 | Tiny normal AllocationProbe allocation | One live then zero after scope | Checked | PASS |
| 11 | Per-request int42, ClaimError, success | Catch-all/standard handler then continue; resources gone before next request | Three scope checks and one progress/count check passed | PASS |
| 12 | Write file then throw ClaimError | File flushed/closed; owner cleaned; removal succeeds | Data reopened and checked; removal0; counters balanced | PASS |
| 13 | Exact/base handlers with throw; and throw e | Bare retains object/type; exact copy retains type but changes identity; copied base slices derived payload | Ten detailed checks passed | PASS |
| 14 | 1000 claims at0/1/50%, all three validators | Rejections0/10/500 and approved sums40000000/39600000/20000000 | All compared and checked | PASS |
| 15 | Negative/nonfinite/exact-cover/zero inputs and bad dimensions | Three validation forms agree; invalid dimensions rejected | Functional driver records33 checks | PASS |
| 16 | 1000000 claims per measured run, 0/1/50%, 5 trials/method | Rejections0/10000/500000 and sums40000000000/39600000000/20000000000 | All30 timed runs checked;60 checksum/count assertions | PASS |

Program counts: main25, rethrow types10, small functional validation33, benchmark60, and three exact-output comparisons:131 runtime evaluations in total. Benchmarks compiled at -O2; other local examples at -O0 with all warning flags. These counts include repeated benchmark configurations, not131 distinct input scenarios. Original B2 prediction commit94ffa2c precedes first execution. Raw output/commands/source hashes are retained under outputs/.

See benchmark_results.md for all actual trials and medians. Timings are observations, not expected pass thresholds. Sanitizer evidence is recorded separately; safe resource/exception runs must actually exit0 with empty sanitizer stderr.
