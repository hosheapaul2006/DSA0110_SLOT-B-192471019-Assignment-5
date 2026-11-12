# Assignment 5 — Reliable Software with Runtime Polymorphism and Exceptions

D Hoshea Paul · Register number **192471019** · **DSA0110**, Object Oriented Programming with C++ · **Slot B** · **CO5** · **C++17**

CO5: design and investigate reliable software using runtime polymorphism, abstract interfaces, exception propagation and resource safety. Nine tasks, 50 marks. The repository contains task-local designs, pseudocode, diagrams, programs, tests and genuine execution evidence. **No assignment report was created**, following the updated faculty instruction supplied by the student. The original assignment PDF is retained only as input in Reference.

## Navigation

| Part A | Design and evidence |
|---|---|
| [A1](PartA/TaskA1/design.md) | [Bill UML](PartA/TaskA1/diagrams/hierarchy.svg), four charge models, [pointer-array pseudocode](PartA/TaskA1/pseudocode.md), switch alternative |
| [A2](PartA/TaskA2/explanation.md) | [Conceptual vtables](PartA/TaskA2/diagrams/conceptual_vtables.svg), binding, destructor experiments, [screenshots](PartA/TaskA2/screenshot_index.md) |
| [A3](PartA/TaskA3/design.md) | [Payer UML](PartA/TaskA3/diagrams/hierarchy.svg), explicit sample policies, [pseudocode](PartA/TaskA3/pseudocode.md), function-pointer/std::function alternatives |
| [A4](PartA/TaskA4/exception_strategy.md) | [Exception hierarchy](PartA/TaskA4/diagrams/exception_hierarchy.svg), error messages, noexcept contracts, status-code alternative |
| [A5](PartA/TaskA5/recovery_design.md) | [Recovery pseudocode](PartA/TaskA5/pseudocode.md), RAII, exact Guard prediction, [screenshots](PartA/TaskA5/screenshot_index.md) |

| Part B | Implementation and evidence |
|---|---|
| [B1](PartB/TaskB1/explanation.md) | [main.cpp](PartB/TaskB1/main.cpp), [supplied TODO](PartB/TaskB1/complete_code.cpp), [12-case matrix and tests](PartB/TaskB1/test_cases.md), [screenshots](PartB/TaskB1/screenshot_index.md) |
| [B2](PartB/TaskB2/explanation.md) | [error driver](PartB/TaskB2/main.cpp), [supplied TODO](PartB/TaskB2/complete_code.cpp), [tests](PartB/TaskB2/test_cases.md), [actual benchmarks](PartB/TaskB2/benchmark_results.md), [screenshots and online limitations](PartB/TaskB2/screenshot_index.md) |
| [B3](PartB/TaskB3/comparison.md) | [dispatch driver](PartB/TaskB3/main.cpp), [tests](PartB/TaskB3/test_cases.md), [actual benchmarks](PartB/TaskB3/benchmark_results.md), [measurement plan](PartB/TaskB3/measurement_plan.md), [screenshots](PartB/TaskB3/screenshot_index.md) |
| [B4](PartB/TaskB4/engineering_decision.md) | [combined main](PartB/TaskB4/main.cpp), [extension](PartB/TaskB4/extensions.hpp), [tests](PartB/TaskB4/test_cases.md), [unchanged-source proof](PartB/TaskB4/outputs/unchanged_sources.json), [screenshots](PartB/TaskB4/screenshot_index.md) |

[Requirement checklist](ASSIGNMENT_CHECKLIST.md) · [Final evidence audit](Reference/final_audit.json) · [Approved demonstration policies](Reference/approved_policies.md)

## Build and execute

Run from the repository root with g++ on PATH. Tested locally using **g++ 14.2.0**, MinGW-w64 UCRT POSIX SEH, Windows 11 AMD64, AMD Ryzen AI 5 340. See [compiler version](Reference/compiler_version.txt), [platform](Reference/platform.txt), [processor](Reference/processor.txt). Ubuntu sanitizer compilation used **g++ 13.3.0**.

```powershell
New-Item -ItemType Directory -Force build
g++ --version
g++ -std=c++17 -Wall -Wextra -Wpedantic PartB/TaskB1/main.cpp -o build/b1.exe
./build/b1.exe
./build/b1.exe --tests
g++ -std=c++17 -Wall -Wextra -Wpedantic PartB/TaskB2/main.cpp -o build/b2.exe
./build/b2.exe --tests
g++ -std=c++17 -Wall -Wextra -Wpedantic PartB/TaskB3/main.cpp -o build/b3.exe
./build/b3.exe --tests
g++ -std=c++17 -Wall -Wextra -Wpedantic PartB/TaskB4/main.cpp -o build/b4.exe
./build/b4.exe --tests
```

Every .cpp is an independent program; do not link all files together. Headers are shared by the full applications. Driver inputs are embedded for reproducibility and transcribed in task-local inputs folders. No keyboard input is needed. `--tests` prints real assertion labels/counts to stderr; assertions run in normal mode too. Compile any other valid source with the same C++17 warning flags. Exact commands, source/dependency hashes, compiler logs, actual stdout/stderr and exit codes are in each task's outputs folder.

Three intentionally ill-formed sources are expected to fail compilation: A2 experiments/destructor_keyword_only.cpp; B1 experiments/root_pure_without_virtual.cpp; B1 experiments/nonvirtual_override_error.cpp. Two nonvirtual-destructor experiments compile with warnings and are executed only in the isolated Ubuntu sanitizer workflow. The correct applications retain virtual destructors.

To reproduce the recorded configurations using Python 3:

```text
python tools/run_all.py
python tools/summarize.py
python tools/audit.py
```

Set `CXX` to the desired compiler path when needed. The verification runner also finds this workspace's portable MinGW compiler automatically. `audit.py` uses Pillow to inspect JPEG metadata. `run_all.py` executes optimized benchmarks and regenerates the self-contained online sources; `summarize.py` derives medians from the new raw timings. Re-running changes observed timings and timestamps; do not interpret older design-decision timing quotations as measurements from a new run.

## Verified results and counting

**27 base source programs:** 24 compile, including two unsafe experiments reserved for sanitizer execution; 22 have successful safe local runs. Three additional base sources intentionally fail compilation. **13 extra self-contained online source copies** also compile/run, including the disclosed B2 memory-only subset. Thus the latest records contain **37 successful compilations**, **35 safe local build/run configurations**, **948 runtime assertion/exact-output evaluations**, and **five expected compiler-diagnostic evaluations** (three errors, two warnings). Repeated checks in flattened copies and the unchanged B4 baseline are included; these are not 948 unique scenarios. Historical retries are not added to these canonical configuration counts.

The full B1 suite has 98 checks and exercises every required bill against every baseline payer. Hand-derived daily bill total is ₹87,700. The preserved supplied TODO has OP ₹1,700, IP ₹47,500 and total ₹49,200; its smaller IP example omits medicines. B2 has 25 checks in its full resource/error driver. B3's ten hand-derived bills total ₹116,700 across virtual, switch and variant processing. B4 reuses all 98 original B1 checks unchanged and adds 22 extension checks. All **33 protected pre-B4 source files** retain their original bytes.

Exact prediction files were committed before first execution: A2 `7c78d17`, A5 `9000c57`, B2 `94ffa2c`. Their observed outputs match character for character after platform newline decoding. Both supplied TODOs retain all text outside the requested regions. The audit verifies chronology, preserved source and completed regions.

The prompt's root pure-function statement requires a correction: `double total() const = 0;` is ill-formed when introducing a new, nonvirtual root function. Omitting `virtual` preserves virtualness when overriding an inherited virtual function. Separate actual compiler/dispatch experiments demonstrate both cases.

## Programiz evidence

**19 distinct functional online cases**, with **24 original screenshots**: 23 successful/annotated execution views and one retained filesystem failure. [Manifest](Reference/programiz_manifest.json) records the exact source submitted and corresponding local output. The 12 ordinary flattened sources expand local includes without altering program logic; B2's thirteenth copy is an explicitly labelled 24-check online subset. Source, output and capture SHA-256 values accompany every successful/annotated online run.

Two B2 console outputs contain an additional standalone `ERROR!` annotation absent from the C++ source and local stdout. The raw text and images retain it; fixed program text matches after excluding that disclosed annotation. The internal cause is unverified. The full B2 driver could not create its relative-path test file online; alternate paths remain unverified. Its unchanged file RAII test passed locally and under Ubuntu ASan, while the online subset demonstrates memory unwinding. The failed screenshot remains visible in B2's index.

All screenshots were individually reviewed. They are native **1920 × 1080 or 2649 × 2788** JPEG browser captures. The available UI font was approximately 14 px; the exposed menu provided no font-size control. View originals at native scale; no screenshot was upscaled or synthetically reconstructed. One blurred capture was rejected and retaken. Four extra source views supplement the principal 19 captures. [Screenshot inventory](Reference/screenshot_inventory.json) records dimensions, hashes, meaning and observed outcomes.

Large benchmark drivers use the [documented smaller online equivalents](Reference/benchmark_online_equivalents.json). Their timing measurements are actual local optimized C++ runs.

## AddressSanitizer

The local MinGW installation lacks ASan/LSan runtimes. Real runs instead used [GitHub Actions Ubuntu 24.04, commit 127e355](https://github.com/hosheapaul2006/DSA0110_SLOT-B-192471019-Assignment-5/actions/runs/43250582265), with **21 cases**: **19 clean exits with empty sanitizer stderr**, **one expected B1 new-delete-type-mismatch diagnostic**, and **one A2 undefined-behavior observation without a sanitizer diagnostic**. The silent A2 run is explicitly not classified as safe. Source and dependency hashes still match. [Summary](Reference/sanitizer_summary.json), [environment](Reference/sanitizer_environment.txt), and task-local sanitizer_* logs preserve actual commands, diagnostics and exit codes.

On a Linux host with sanitizer runtimes:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -g -O1 -fsanitize=address -fno-omit-frame-pointer PartB/TaskB4/main.cpp -o b4_asan
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1:alloc_dealloc_mismatch=1:new_delete_type_mismatch=1 ./b4_asan
python3 tools/sanitizers.py
```

The script executes unsafe variants in separate processes, preserves their outcomes, and never calls silent undefined behavior clean. Leak detection was enabled. Allocation failure in B2/B4 is safely injected before an OS allocation, explicitly labelled; no memory-exhaustion experiment was performed.

## Actual performance measurements

Five trials per group, **milliseconds**, local GCC 14.2.0 **-O2**. Raw CSV, commands, assertion checksums and every trial are retained in B2/B3 outputs; [derived statistics](Reference/benchmark_summary.json) contain no predicted timings.

| Logical bills | Virtual median | Switch median | Variant/std::visit median |
|---|---:|---:|---:|
| 1,000 | 0.0022 | 0.0014 | 0.0014 |
| 1,000,000 | 3.8387 | 2.0086 | 1.7429 |
| 10,000,000 | 29.8849 | 21.8108 | 17.6913 |

| Invalid claims (1,000,000 total) | B2 optional | B2 exceptions | B3 return code | B3 exceptions |
|---|---:|---:|---:|---:|
| 0% | 0.8319 | 0.8322 | 0.8057 | 0.8023 |
| 1% | 0.8164 | 13.5408 | 0.8087 | 12.1752 |
| 50% | 3.9914 | 588.6533 | 4.0447 | 578.3921 |

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 PartB/TaskB2/benchmark.cpp -o build/b2_bench
./build/b2_bench --tests
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 PartB/TaskB3/benchmark.cpp -o build/b3_bench
./build/b3_bench --tests
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 PartB/TaskB3/validation_benchmark.cpp -o build/validation_bench
./build/validation_bench --tests
```

Dispatch uses at most 65,536 resident records and a 64 MiB requested-payload guard; larger logical workloads replay identical full/partial batches. These are 10 million total calculations, not 10 million distinct resident objects or complete hospital transactions. The three representations receive equivalent data; independent totals catch arithmetic/rounding differences. Timed loops exclude construction, allocation, input generation, logging and cleanup. Variant here contains the production derived bill classes and therefore retains their inherited polymorphic storage. Cache behavior and this bounded working set affect results. These measurements support the task-local engineering decision, not a production throughput guarantee.

## Submission and outstanding student actions

The existing repository is intentionally unchanged in name. The PDF requests `DSA01-192471019-Assignment5`; the supplied repository is `DSA0110_SLOT-B-192471019-Assignment-5`. Review this naming discrepancy with faculty. The named official SIMATS pseudocode guide was not supplied or found; the student approved conventional structured pseudocode, with no claim of verified official formatting compliance.

The student must personally read and execute the programs, review the evidence, and prepare for viva. **Google Classroom has not been submitted or its destination verified.** The student chose to submit personally. Submit this repository link after that review; no report attachment is required under the updated instruction:

https://github.com/hosheapaul2006/DSA0110_SLOT-B-192471019-Assignment-5
