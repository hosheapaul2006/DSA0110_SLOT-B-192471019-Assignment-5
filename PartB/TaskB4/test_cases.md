# B4 executed extension and regression checks

| ID | Scenario | Expected | Actual | Status |
|---|---|---|---|---|
| 1 | Diagnostic scan6000+report500 | Total6500 | Checked | PASS |
| 2 | Corporate80%, cap100000 on Diagnostic | Covered5200, patient1300 | Checked | PASS |
| 3 | Existing Insurance on new Diagnostic | Covered5200 | Checked | PASS |
| 4 | Corporate on unchanged DayCare250000 | Covered100000, patient150000 | Checked | PASS |
| 5 | Corporate exact cap boundary125000; zero bill;0%/100% policies;zero cap | Correct boundary coverage | Five checks passed | PASS |
| 6 | Under-cover Insurance on Diagnostic | ClaimError audited/rethrown/caught; bill remains | Message, pointer, size and total checked | PASS |
| 7 | Unknown government diagnostic package | PackageError, no invented allowance | Checked; later corporate retry succeeds | PASS |
| 8 | Negative scan/report, excessive total, percentage101, negative cap | ValidationError; cleanup | Five rejection checks passed | PASS |
| 9 | Injected allocation failure in extended run | bad_alloc caught, no leaked probe, existing bill retained | Checked | PASS |
| 10 | End extended scope | No live bills/probes | Checked | PASS |
| 11 | All33 existing .cpp/.hpp files | Exact same bytes as pre-extension snapshot | Every SHA256 matched | PASS |
| 12 | Combined driver | Earlier98 checks plus22 new checks |120 evaluations passed; baseline stdout prefix unchanged | PASS |
| 13 | Extension-only driver | Same new behavior independently |22 evaluations passed | PASS |
| 14 | All earlier safe examples, TODOs, diagnostics and large -O2 runs after extension | All checks continue to succeed | Complete run_all regression succeeded | PASS |
| 15 | Extended thrown/caught paths under ASan | Exit0, empty sanitizer stderr | Ubuntu ASan main and extension each exit0 with empty stderr; workflow43250582265 | PASS |

The two B4 drivers record142 runtime evaluations in total, including repeated baseline/extension checks. No existing implementation class was changed. Original source baseline and post-extension proof are Reference/baseline_before_B4.json and outputs/unchanged_sources.json. Regression evidence is outputs/regression_summary.json; individual actual commands/results remain in each task's outputs/.
