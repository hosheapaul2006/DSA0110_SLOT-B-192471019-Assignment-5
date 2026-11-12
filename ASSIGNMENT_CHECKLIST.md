# Assignment 5 requirement checklist

D Hoshea Paul | 192471019 | DSA0110 | Slot B | CO5

Created from all 16 PDF pages and the current user prompt before implementation. PASS requires evidence; intentionally bad examples are classified separately. No assignment report will be created.

| Task/ID | Requirement | Source/evidence/screenshot | Status |
|---|---|---|---|
| A1.01 | Common patient/bill identity and total/type/print/destruction contract | [design](PartA/TaskA1/design.md), [UML](PartA/TaskA1/diagrams/hierarchy.md), [pseudocode](PartA/TaskA1/pseudocode.md) | PASS |
| A1.02 | Abstract Bill and four bill categories; charges correctly modeled | [design](PartA/TaskA1/design.md), [UML](PartA/TaskA1/diagrams/hierarchy.md), [pseudocode](PartA/TaskA1/pseudocode.md) | PASS |
| A1.03 | UML marks pure/virtual/overridden methods and virtual destruction | [design](PartA/TaskA1/design.md), [UML](PartA/TaskA1/diagrams/hierarchy.md), [pseudocode](PartA/TaskA1/pseudocode.md) | PASS |
| A1.04 | Pseudocode totals array of Bill pointers using runtime dispatch | [design](PartA/TaskA1/design.md), [UML](PartA/TaskA1/diagrams/hierarchy.md), [pseudocode](PartA/TaskA1/pseudocode.md) | PASS |
| A1.05 | Switch/type-code alternative code or pseudocode | [design](PartA/TaskA1/design.md), [UML](PartA/TaskA1/diagrams/hierarchy.md), [pseudocode](PartA/TaskA1/pseudocode.md) | PASS |
| A1.06 | O(n) processing per-object storage and extension-change analysis | [design](PartA/TaskA1/design.md), [UML](PartA/TaskA1/diagrams/hierarchy.md), [pseudocode](PartA/TaskA1/pseudocode.md) | PASS |
| A2.01 | Early/static versus late/dynamic binding through Bill pointer | [analysis](PartA/TaskA2/explanation.md), [vtables](PartA/TaskA2/diagrams/conceptual_vtables.md), [tests](PartA/TaskA2/test_cases.md) | PASS |
| A2.02 | Conceptual vtable diagrams Bill OP IP with dispatch route | [analysis](PartA/TaskA2/explanation.md), [vtables](PartA/TaskA2/diagrams/conceptual_vtables.md), [tests](PartA/TaskA2/test_cases.md) | PASS |
| A2.03 | Vtables explicitly described as implementation technique, not mandated layout | [analysis](PartA/TaskA2/explanation.md), [vtables](PartA/TaskA2/diagrams/conceptual_vtables.md), [tests](PartA/TaskA2/test_cases.md) | PASS |
| A2.04 | Virtual destructor rationale and unsafe deletion consequences | [analysis](PartA/TaskA2/explanation.md), [vtables](PartA/TaskA2/diagrams/conceptual_vtables.md), [tests](PartA/TaskA2/test_cases.md) | PASS |
| A2.05 | Exact supplied source and prediction committed before first execution | [analysis](PartA/TaskA2/explanation.md), [vtables](PartA/TaskA2/diagrams/conceptual_vtables.md), [tests](PartA/TaskA2/test_cases.md) | PASS |
| A2.06 | Actual original output and exact comparison with immutable prediction | [analysis](PartA/TaskA2/explanation.md), [vtables](PartA/TaskA2/diagrams/conceptual_vtables.md), [tests](PartA/TaskA2/test_cases.md) | PASS |
| A2.07 | Nonvirtual destructor variation prediction saved before run | [analysis](PartA/TaskA2/explanation.md), [vtables](PartA/TaskA2/diagrams/conceptual_vtables.md), [tests](PartA/TaskA2/test_cases.md) | PASS |
| A2.08 | Retained destructor override actual compile error; controlled runnable UB variant disclosed | [analysis](PartA/TaskA2/explanation.md), [vtables](PartA/TaskA2/diagrams/conceptual_vtables.md), [tests](PartA/TaskA2/test_cases.md) | PASS |
| A2.09 | Actual ASan observation; silent UB never called safe | PartA/TaskA2/ | PENDING |
| A2.10 | dynamic_cast alternative and up-to-k type testing/time/space comparison | [analysis](PartA/TaskA2/explanation.md), [vtables](PartA/TaskA2/diagrams/conceptual_vtables.md), [tests](PartA/TaskA2/test_cases.md) | PASS |
| A3.01 | Abstract Payer amountCovered(const Bill&) interface and responsibilities | PartA/TaskA3/ | PENDING |
| A3.02 | SelfPay Insurance GovernmentScheme policy data and documented semantics | PartA/TaskA3/ | PENDING |
| A3.03 | Coverage limit co-payment patient contribution and rejection handling | PartA/TaskA3/ | PENDING |
| A3.04 | CorporatePayer extension path without editing existing payer classes | PartA/TaskA3/ | PENDING |
| A3.05 | Payer UML hierarchy and structured pseudocode | PartA/TaskA3/ | PENDING |
| A3.06 | Function-pointer/std::function alternatives with state-storage examples | PartA/TaskA3/ | PENDING |
| A3.07 | O(1) fixed-policy invocation storage captured state and extensibility comparison | PartA/TaskA3/ | PENDING |
| A4.01 | Negative/invalid charges claim-above-cover unknown-package missing-patient allocation errors | PartA/TaskA4/ | PENDING |
| A4.02 | Exception hierarchy derived from std::exception with meaningful messages | PartA/TaskA4/ | PENDING |
| A4.03 | Throwing versus noexcept functions table and justified contracts | PartA/TaskA4/ | PENDING |
| A4.04 | Destructors and stack-unwinding no-throw rationale | PartA/TaskA4/ | PENDING |
| A4.05 | Role of invalid_argument out_of_range bad_alloc | PartA/TaskA4/ | PENDING |
| A4.06 | Status-code/bool alternative and normal/error-path time/space/propagation analysis | PartA/TaskA4/ | PENDING |
| A5.01 | Validation catch/log/rethrow/final-report boundaries and genuine recovery plan | PartA/TaskA5/ | PENDING |
| A5.02 | RAII stack unwinding unique_ptr file and memory resource safety | PartA/TaskA5/ | PENDING |
| A5.03 | Exact supplied Guard source and prediction committed before execution | PartA/TaskA5/ | PENDING |
| A5.04 | Actual exact output and destructor-order explanation | PartA/TaskA5/ | PENDING |
| A5.05 | Structured recovery pseudocode including bare throw | PartA/TaskA5/ | PENDING |
| A5.06 | Manual delete-in-catches alternative with leak-path counts and maintainability | PartA/TaskA5/ | PENDING |
| B1.01 | Implement four Bill categories and three baseline payers from Part A | PartB/TaskB1/ | PENDING |
| B1.02 | Twelve bill/payer combinations with inputs total coverage patient share expected/actual | PartB/TaskB1/ | PENDING |
| B1.03 | Independent hand calculations for representative bill totals and contributions | PartB/TaskB1/ | PENDING |
| B1.04 | Retain/recover bill ownership across payment rejection | PartB/TaskB1/ | PENDING |
| B1.05 | Compare implementation to Part A and explain changes | PartB/TaskB1/ | PENDING |
| B1.06 | Actual root pure-specifier-without-virtual compile error | PartB/TaskB1/ | PENDING |
| B1.07 | Inherited virtual omission remains virtual demonstration | PartB/TaskB1/ | PENDING |
| B1.08 | Genuinely nonvirtual function with retained override actual compile error | PartB/TaskB1/ | PENDING |
| B1.09 | Controlled nonvirtual version without override demonstrates static binding | PartB/TaskB1/ | PENDING |
| B1.10 | Unsafe nonvirtual destruction isolated ASan evidence and repaired design | PartB/TaskB1/ | PENDING |
| B1.11 | Complete supplied TODO1-3 changing only requested regions | PartB/TaskB1/ | PENDING |
| B1.12 | Exact TODO output OP1700 IP47500 Total49200 | PartB/TaskB1/ | PENDING |
| B1.13 | Template/separate-container alternative and mixed-list/time/space analysis | PartB/TaskB1/ | PENDING |
| B1.14 | Normal boundary invalid and ownership tests | PartB/TaskB1/ | PENDING |
| B2.01 | Negative charge claim-over-cover unknown package and missing patient tests | PartB/TaskB2/ | PENDING |
| B2.02 | Invalid stod text invalid_argument and out-of-range numeric conversion | PartB/TaskB2/ | PENDING |
| B2.03 | Safe controlled bad_alloc test clearly identifies injected failure | PartB/TaskB2/ | PENDING |
| B2.04 | Catch-all unknown exception keeps processing; destructor traces and clean ownership | PartB/TaskB2/ | PENDING |
| B2.05 | Exact supplied validate/process prediction committed before first execution | PartB/TaskB2/ | PENDING |
| B2.06 | Actual prediction output character comparison and rethrow-handler explanation | PartB/TaskB2/ | PENDING |
| B2.07 | Complete supplied approve/settle TODO1-2 only | PartB/TaskB2/ | PENDING |
| B2.08 | Exact TODO expected approved/audit/rejected/error output | PartB/TaskB2/ | PENDING |
| B2.09 | Bare throw versus throw e copy; correct same-type and base-slicing experiments | PartB/TaskB2/ | PENDING |
| B2.10 | One-million-claim optional vs exception benchmarks 0/1/50 percent invalid | PartB/TaskB2/ | PENDING |
| B2.11 | Actual commands inputs raw trials medians checksums and interpretation | PartB/TaskB2/ | PENDING |
| B2.12 | Normal boundary invalid exception tests and genuine Programiz evidence | PartB/TaskB2/ | PENDING |
| B3.01 | Same ten bills agree under virtual and switch totals | PartB/TaskB3/ | PENDING |
| B3.02 | Virtual/switch actual -O2 benchmarks for 1000 1000000 10000000 logical bills | PartB/TaskB3/ | PENDING |
| B3.03 | Equivalent inputs safeguards observable results repeat trials consistent units | PartB/TaskB3/ | PENDING |
| B3.04 | One-million validations return-code vs exception at 0/1/50 percent invalid | PartB/TaskB3/ | PENDING |
| B3.05 | Variant/std::visit functional implementation and 10000000-bill timing comparison | PartB/TaskB3/ | PENDING |
| B3.06 | sizeof/storage largest alternative/tag/vptr/unused-fields analysis | PartB/TaskB3/ | PENDING |
| B3.07 | Normal/error-path measured trade-off and limitations distinguished from Big-O | PartB/TaskB3/ | PENDING |
| B3.08 | Memory-bounded batching and excluded/included timing scope explicitly recorded | PartB/TaskB3/ | PENDING |
| B4.01 | New DiagnosticBill and CorporatePayer without edits to existing classes | PartB/TaskB4/ | PENDING |
| B4.02 | Pre-extension source hashes/commit and actual unchanged-code proof | PartB/TaskB4/ | PENDING |
| B4.03 | Earlier tests rerun and unchanged outputs verified | PartB/TaskB4/ | PENDING |
| B4.04 | Actual new bill total coverage and patient contribution tests | PartB/TaskB4/ | PENDING |
| B4.05 | Extended application thrown/caught exceptions and clean ASan evidence | PartB/TaskB4/ | PENDING |
| B4.06 | Production decision for 15 hospitals 200000 bills/day informed by measurements | PartB/TaskB4/ | PENDING |
| B4.07 | Configuration/rule-driven plug-in alternative pseudocode | PartB/TaskB4/ | PENDING |
| B4.08 | Rule interpretation/storage/deployment/extensibility/risk/testing comparison | PartB/TaskB4/ | PENDING |
| G.01 | C++17 warnings actual compiler version exact commands and independent execution | Final verification | PENDING |
| G.02 | Prediction chronology immutable files and TODO-only preservation audit | Final verification | PENDING |
| G.03 | Real local/Ubuntu sanitizer and benchmark evidence source hashes and timestamps | Final verification | PENDING |
| G.04 | All suitable programs run on real Programiz with exact-source/output verification | Final verification | PENDING |
| G.05 | Native high-resolution readable screenshots reviewed individually and indexed per task | Final verification | PENDING |
| G.06 | At least nine meaningful progressive task commits and actual Git identity | Final verification | PENDING |
| G.07 | README complete task navigation exact build/run/test/sanitizer/benchmark instructions | Final verification | PENDING |
| G.08 | All files pushed main clean and local/remote contents verified | Final verification | PENDING |
| G.09 | No report; supplied PDF report wording explicitly superseded by user instruction | Final verification | PENDING |
| G.10 | Existing repository naming discrepancy flagged without renaming | Final verification | PENDING |
| G.11 | Official SIMATS pseudocode guide unavailable; user-approved structured fallback disclosed | Final verification | PENDING |
| G.12 | Student personally executes/reviews tasks and prepares for viva | Final verification | PENDING |
| G.13 | Correct Classroom destination and final user submission | Final verification | PENDING |
