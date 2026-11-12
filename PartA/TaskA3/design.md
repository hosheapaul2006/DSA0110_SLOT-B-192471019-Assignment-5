# A3 - Abstract payer policies

Payer declares `amountCovered(const Bill&)` as pure virtual and has a virtual noexcept destructor. Its result is the payer's approved contribution, never the patient's contribution. A shared settlement service checks 0 <= covered <= bill.total() and computes patientShare = total-covered. A rejection leaves the original bill and an unresolved attempt available for correction; it never silently substitutes zero coverage or deletes the bill.

## Explicit user-approved demonstration rules

| Payer | Stored policy | Coverage behavior |
|---|---|---|
| SelfPay | No policy state | covered=0; patient pays the entire total |
| Insurance | coverLimit=50000; coPaymentPercent=20 | proposed=80% of total, rounded to paise; reject with ClaimError if proposed exceeds cover; otherwise cover proposed |
| GovernmentScheme | Package-code/allowance table | covered=min(total, package allowance); reject unknown code with PackageError |
| CorporatePayer (later extension) | fraction=80%; cap=100000 | covered=min(80% of total, cap); no changes to existing payer classes |

Government allowances are OP_STD=1500, IP_STD=40000, DC_STD=20000, ER_STD=10000 rupees. They are illustrative rates, not real scheme rules. A package allowance does not authorize paying more than the actual bill. Insurance's cover applies to the proposed insurer share, not the gross charge, and a rejected claim is not automatically split into partial claims. The supplied B2 TODO independently defines approve(billed,cover) using gross billed>cover; that exact exercise policy stays unchanged and is not presented as identical to this application policy.

All limits must be finite/nonnegative; percentages must be within 0..100; package codes must be nonempty and allowances valid. Zero coverage is permitted. A cent-rounding helper will bound demonstration monetary inputs to 0..1e12 rupees, reject nonfinite/unrepresentable values and round to nearest paise. These technical limits and rounding rules are explicit; they are not a production reimbursement contract.

## Expected values before implementation

| Bill total | SelfPay covered / patient | Insurance covered / patient | Government covered / patient |
|---|---|---|---|
| OP 1700 | 0 / 1700 | 1360 / 340 | 1500 / 200 |
| IP 50000 | 0 / 50000 | 40000 / 10000 | 40000 / 10000 |
| Day care 25000 | 0 / 25000 | 20000 / 5000 | 20000 / 5000 |
| Emergency 11000 | 0 / 11000 | 8800 / 2200 | 10000 / 1000 |

Insurance contributions sum to 70160 and patient shares to 17540. Government contributions sum to 71500 and patient shares to 16200. Each pair sums to the 87700 daily bill total. These are hand-derived expectations, not claims of executed tests.

## Extension path

CorporatePayer is a new public Payer subtype using only Bill::total. Implement it later in a new B4 header, construct it at the application boundary, and supply it through Payer&. Existing SelfPay, Insurance, GovernmentScheme, Bill classes and settlement code remain unchanged. A3's diagram/pseudocode demonstrates the design path; B4 will provide the actual C++ extension and before/after source-hash evidence.

## Function-pointer and std::function alternatives

```text
RECORD InsuranceState: coverLimit, coPaymentPercent
FUNCTION INSURANCE_POLICY(state, bill): apply the same validated insurance rule
RECORD FunctionPolicy: functionPointer, borrowedStatePointer
covered <- selected.functionPointer(selected.state, bill)

state <- owned validated InsuranceState
callable <- function capturing state BY VALUE
covered <- callable(bill)     // std::function<double(const Bill&)> equivalent
```

A plain function pointer does not store a coverage limit or capture local state; add an explicit context object/pointer, use globals (with undesirable shared-state coupling), or choose a separate function for each hardcoded policy. A borrowed context must outlive calls. std::function can own a value-captured policy and type-erases its callable, but reference captures can dangle and copying it can copy state. Storage may use a small-buffer optimization or heap allocation; neither size nor allocation behavior is guaranteed. An abstract object stores state in fields and communicates ownership through a base pointer/unique_ptr.

Fixed policies require O(1) arithmetic/invocation in each design. A package table with P entries costs O(log P) map lookup and up to O(code length) comparisons; with the four fixed packages this is bounded, not a general guarantee for arbitrarily many rules. Storage is O(1) per fixed Insurance/SelfPay object, plus text/metadata, and O(P) for the government table. A function pointer alone is smaller than a stateful policy object but omits required state. std::function adds a type-erased wrapper, possible allocated capture storage and copy-management cost. B1 will execute equivalent policy forms and record actual sizeof values; no unmeasured speed advantage is assumed.
