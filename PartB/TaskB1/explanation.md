# B1 - Validated polymorphic billing

## Implementation versus Part A

The shared declarations are in billing.hpp, payers.hpp and errors.hpp. The four bill types implement the A1 formulas and pure virtual noexcept total/type. The three baseline payers implement the exact approved A3 rules. Bill/Payer destructors are virtual and noexcept. Patient identity is an owned value; package allowance data belongs to GovernmentScheme. Conversion/rounding and the 0..1e12-rupee demonstration range follow A4's explicit contract. The supplied TODO keeps its original double-based, minimal exercise structure instead of receiving unrelated validation changes.

The main ownership refinement from A1 is a vector<unique_ptr<Bill>> ledger, permitting later extensions without a fixed owner-array size. The required base-pointer processing still uses an actual `const Bill* bills[]` array, with views into stable separately owned objects. Moving unique_ptr slots does not move their Bill objects. Duplicate/null insertion is rejected. Bills are noncopyable to prevent slicing; a noexcept move supports by-value variant experiments without duplicating ownership. A moved-from bill is used only for destruction in the experiments, not as a new valid patient record.

test_suite.hpp contains the reusable baseline tests. main.cpp executes them; B4 will rerun the unchanged same suite after adding new classes. Patients, bill IDs, package codes, charges and expected coverage are embedded fixtures. inputs/bill_payer_cases.csv transcribes those inputs and hand expectations for review; the driver does not claim to read that CSV as stdin. Actual observed matrix rows are saved separately in outputs/bill_payer_actual.csv.

The twelve results cover every required bill category with SelfPay, Insurance and GovernmentScheme. Hand totals and the explicit coverage/patient shares are in the test table. The day sum 87700 excludes duplicate counting across payers. Additional cases check percentage/cover boundaries, zero charges, fractional emergency time, paise rounding, invalid fields, missing patients, unknown packages, duplicate IDs, null arrays/owners, overflows, payer overpayment and cleanup after constructor failures.

## Rejection, logging and recovery

Payers borrow const bills. settle verifies the contribution remains within the bill total; a malicious/incorrect payer cannot return negative, nonfinite or excessive payment. settleWithAudit catches BillingError, logs best-effort and uses bare throw. An explicit broken-stream test proves logging failure does not replace ClaimError. The ledger still owns the same bill at the same address after rejection. A later SelfPay settlement succeeds. This is in-process resource safety and recovery, not durable database settlement.

## Runtime-binding experiments and corrected premise

| File under experiments/ | Actual observation |
|---|---|
| root_pure_without_virtual.cpp | Root `double total() const = 0` without virtual is ill-formed; genuine compiler error |
| inherited_virtual_omission.cpp | Matching inherited virtual declared without repeating virtual remains virtual; calls derived total 500 |
| nonvirtual_override_error.cpp | Replacing pure total with a real nonvirtual body while leaving derived override is rejected |
| nonvirtual_binding.cpp | Removing override only from derived totals makes the controlled version valid; base-pointer totals are all 0 |
| nonvirtual_destructor.cpp | Supplied completed program with only the base destructor made nonvirtual; compile-only locally, isolated ASan execution required |

The prompt's blanket statement that a pure-specifier alone keeps a new root declaration virtual is incorrect. It is inherited virtualness that allows keyword omission in the second case. The PDF's requested compile error genuinely occurs for the root case. In the valid static-binding version, type() remains virtual, so labels still print OP/IP while total() statically returns 0. Correct production classes retain virtual total and virtual destruction; unsafe experiments never modify the main implementation.

complete_code.cpp replaces only TODO 1-3 in the original program; the preservation record reconstructs it from the untouched reference plus those insertions. Its InPatient intentionally excludes medicines and totals 47500. Real warning/error output and exact execution commands are retained under outputs/.

## Template and policy alternatives

template_alternative.cpp implements four nonvirtual value types, each in its own typed vector, and a generic groupTotal<T>. These calls are statically selected and need no virtual interface; they match the same 87700 total, check an empty container and reject a negative component. The model includes validation for the values it owns. It cannot put all four different value types into one ordinary homogeneous vector without adding a union/variant/type erasure or restoring a common polymorphic interface. Processing remains O(n), with O(n) objects and one container per type; value-type storage can omit a vptr but uses separate collection metadata. Templates can also increase instantiated code size. Using a template on a virtual member alone would not automatically remove C++ virtual semantics; these alternative classes are deliberately nonvirtual.

payer_alternatives.cpp executes equivalent abstract-object, function-pointer-plus-context and value-captured std::function insurance policies. It checks equal coverage, equal over-cover rejection, and the capture's independence from later context mutation. Actual sizeof values are printed, excluding any allocated capture storage. The context values are validated demonstration inputs; a general function-pointer API also needs its own input/state validation and lifetime rules. No speed result is inferred from wrapper sizeof. O(1) policy arithmetic and the map lookup's fixed/sized behavior follow A3; B3 supplies timings.

## Sanitizers

Safe owner/exception paths and the intentionally nonvirtual-delete copy are run in isolated Ubuntu ASan. A clean run requires actual exit 0 and empty sanitizer stderr. The deliberately bad copy requires its real diagnostic and is never called correct. The final raw records and interpretation are linked in the completed test table when available. A2 already demonstrates that a different bad delete can evade ASan; sanitizer absence is not proof of defined behavior.
