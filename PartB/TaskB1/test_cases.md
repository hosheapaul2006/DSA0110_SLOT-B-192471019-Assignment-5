# B1 verified matrix and experiments

| Bill / payer | Charges | Expected total / covered / patient | Actual total / covered / patient | Status |
|---|---|---|---|---|
| OP / SelfPay | consultation=500;tests=1200 | 1700 / 0 / 1700 | 1700.00 / 0.00 / 1700.00 | PASS |
| OP / Insurance | consultation=500;tests=1200 | 1700 / 1360 / 340 | 1700.00 / 1360.00 / 340.00 | PASS |
| OP / Government | consultation=500;tests=1200 | 1700 / 1500 / 200 | 1700.00 / 1500.00 / 200.00 | PASS |
| IP / SelfPay | days=3;perDay=2500;procedures=40000;medicines=2500 | 50000 / 0 / 50000 | 50000.00 / 0.00 / 50000.00 | PASS |
| IP / Insurance | days=3;perDay=2500;procedures=40000;medicines=2500 | 50000 / 40000 / 10000 | 50000.00 / 40000.00 / 10000.00 | PASS |
| IP / Government | days=3;perDay=2500;procedures=40000;medicines=2500 | 50000 / 40000 / 10000 | 50000.00 / 40000.00 / 10000.00 | PASS |
| DayCare / SelfPay | package=25000 | 25000 / 0 / 25000 | 25000.00 / 0.00 / 25000.00 | PASS |
| DayCare / Insurance | package=25000 | 25000 / 20000 / 5000 | 25000.00 / 20000.00 / 5000.00 | PASS |
| DayCare / Government | package=25000 | 25000 / 20000 / 5000 | 25000.00 / 20000.00 / 5000.00 | PASS |
| Emergency / SelfPay | base=3000;hours=4;hourly=2000 | 11000 / 0 / 11000 | 11000.00 / 0.00 / 11000.00 | PASS |
| Emergency / Insurance | base=3000;hours=4;hourly=2000 | 11000 / 8800 / 2200 | 11000.00 / 8800.00 / 2200.00 | PASS |
| Emergency / Government | base=3000;hours=4;hourly=2000 | 11000 / 10000 / 1000 | 11000.00 / 10000.00 / 1000.00 | PASS |

The full baseline suite records 98 executed checks: these 12 combinations each have total, coverage, patient-share and conservation checks (48), plus 50 ownership, recovery, boundary and invalid-input checks. Main stdout and stderr retain the real matrix/labels. These are embedded driver inputs, also transcribed to inputs/bill_payer_cases.csv.

| Additional program / experiment | Expected | Actual | Status |
|---|---|---|---|
| Completed supplied TODO | OP Ravi:1700; IP Meena:47500; Total49200, exact spaces/newlines | Exact comparison matched | PASS |
| Root pure total without virtual | Compiler error for nonvirtual pure declaration | Genuine GCC error retained | PASS |
| Inherited virtual, keyword omitted | Dynamic total500 | One check passed | PASS |
| Nonvirtual total with derived override retained | Compiler error | Genuine GCC error retained | PASS |
| Controlled nonvirtual total without override | OP0, IP0, Total0 | Exact comparison matched | PASS |
| Nonvirtual destructor copy | Compile warning; isolated ASan execution | Local warning captured; ASan result not yet retrieved | PENDING |
| Template separate containers | Four expected totals and day87700; empty0; invalid rejection | Four checks passed | PASS |
| Policy alternatives | Three equivalent cover/rejection rules; capture state independent | Three checks passed | PASS |

The safe programs record 108 runtime check evaluations. Three additional compiler-diagnostic checks cover the two intentional errors and one warning/compile-only unsafe source. Sanitizer observations are counted separately.
