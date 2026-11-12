# A2 - Runtime binding and virtual destruction

Early binding selects a nonvirtual member from the expression's static type. Late binding invokes the final overrider for a virtual call using the object's dynamic type. In the supplied source, b->show() selects Bill::show, but its unqualified total() call remains virtual and reaches OPBill::total. Making a wrapper nonvirtual does not make all calls inside it static.

The original output prediction is immutable in prediction_before_execution.md and outputs/predict_output_predicted.txt. Virtual deletion executes the OPBill destructor body before Bill's destructor body, preserving the final space/newline. Conceptual vtables for the planned Bill/OutPatientBill/InPatientBill are in diagrams/conceptual_vtables.md; they are illustrative rather than a standard-mandated memory layout.

## Destructor experiments

experiments/destructor_keyword_only.cpp removes only virtual from the supplied base destructor. Its derived destructor still says override, so compilation must fail: it no longer overrides a virtual destructor. experiments/nonvirtual_destructor.cpp additionally removes that destructor's override to make a controlled runnable experiment. These are the only edits. That second program is compiled locally but executed only in isolated AddressSanitizer. It has undefined behavior and no guaranteed output. The pre-execution unsafe prediction expressly allows no sanitizer diagnostic and does not call that outcome safe.

layout_probe.cpp safely measures the shapes of the nonvirtual-destructor classes without deleting through the base. Equal sizeof/alignment can explain why a size-mismatch detector fails to diagnose the bad delete; absence of a diagnostic cannot establish defined behavior. Actual sanitizer observations will be retained in this folder's outputs/ and interpreted separately from genuine clean runs.

The [virtual function rules](https://eel.is/c++draft/class.virtual), [abstract-class rules](https://eel.is/c++draft/class.abstract) and [delete-expression rules](https://eel.is/c++draft/expr.delete) support these distinctions. A root declaration `double total() const = 0` without virtual is ill-formed; an override of an inherited virtual may omit the keyword and remains virtual. B1 will preserve both real compiler outcomes instead of claiming that the pure-specifier alone makes a new root declaration virtual.

## Manual dynamic_cast alternative

dynamic_cast_alternative.cpp gives Bill a nonvirtual total and retains a virtual destructor so the base is still polymorphic for RTTI. Its dispatcher tests OP then IP and calls through the concrete pointer. The example verifies base total=0 but manual totals=500/1000 and rejects an unsupported type. Extending it requires another type test, unlike an unchanged virtual-call processing loop.

For k candidates, the manual dispatcher may perform up to k casts. Cast complexity depends on the implementation/hierarchy; for this fixed simple hierarchy it is reasonable to model each cast as a constant-cost operation, making the chain O(k). A conventional virtual call uses one dispatch, typically O(1). No language-mandated timing bound or speed claim is inferred. Retaining a virtual destructor means RTTI-based objects still commonly contain a vptr; replacing only total's dispatch does not remove that storage or RTTI tables. Derived data, alignment, shared type metadata and pointer collections must also be counted. Actual timing comparisons are deferred to B3.
