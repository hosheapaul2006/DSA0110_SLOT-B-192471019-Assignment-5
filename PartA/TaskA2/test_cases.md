# A2 execution evidence

| ID | Scenario | Expected | Actual | Status |
|---|---|---|---|---|
| 1 | Exact supplied Bill/OPBill | 500, derived destructor, base destructor; final space/newline | Exact comparison matched | PASS |
| 2 | Remove only virtual from ~Bill, retain ~OPBill override | Compiler rejects invalid override | Actual GCC diagnostic retained | PASS |
| 3 | Also remove derived destructor override | Compiles with nonvirtual-delete warning; unsafe execution isolated | Actual warning captured; not run locally | PASS |
| 4 | Safe layout probe | Virtual total 500; actual ABI sizes/alignment printed | One check passed; sizeof/alignof both 8 on local compiler | PASS |
| 5 | Manual RTTI dispatch | Static base totals 0, manual OP/IP totals 500/1000 | Two checks passed | PASS |
| 6 | Unsupported manual-dispatch type | invalid_argument | Exception observed | PASS |
| 7 | Isolated ASan controlled bad delete | Undefined behavior; diagnostic not guaranteed | Ubuntu exit 0, stdout 500 ~Bill with final space/newline; sanitizer stderr empty, explicitly labelled UB observation | PASS |

The original prediction was committed in 7c78d17 before first execution and is unchanged. Local records contain five runtime check evaluations plus two expected compiler-diagnostic checks. An observed silent UB case will be classified separately from clean safe executions.
