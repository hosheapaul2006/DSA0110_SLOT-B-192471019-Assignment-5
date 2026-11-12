# B4 - Engineering decision for the hospital group

## Actual extension and regression result

extensions.hpp adds DiagnosticBill and CorporatePayer only. DiagnosticBill’s ₹6,500 total produces ₹5,200 corporate coverage and ₹1,300 patient contribution. The existing Insurance also covers ₹5,200 on the new bill. CorporatePayer works with an unchanged DayCareBill of ₹250,000 and caps coverage at ₹100,000, leaving ₹150,000. Existing government configuration deliberately rejects the unknown diagnostic package instead of inventing an allowance. The bill remains owned and a valid retry succeeds.

All 33 protected pre-extension C++ sources/headers match their recorded hashes in outputs/unchanged_sources.json. The baseline source commit was 52ac193; snapshot/design commit ba01049 preceded the new classes. No existing bill, payer, exception or experiment class was edited. B4 main reruns the exact B1 test suite and its stdout prefix matches byte-for-byte. The separate extension driver repeats the new behavior independently. outputs/regression_summary.json records the full post-extension rerun of earlier safe programs, TODOs, compiler diagnostics and optimized benchmarks; unsafe drivers stay isolated.

The extended runs throw/catch ClaimError, PackageError and injected bad_alloc while retaining the bill, then release all owners. The final test table links actual clean Ubuntu ASan results for both extended drivers; counters alone are not treated as sanitizer evidence.

## Measurements informing the decision

The final post-extension dataset uses Windows GCC 14.2.0, C++17, -O2, five rotated trials and medians. For 10,000,000 logical bill calculations over the bounded 65,536-record working set, medians were virtual 29.8849 ms, switch 21.8108 ms and variant/std::visit 17.6913 ms. This measures traversal and arithmetic on these layouts; it excludes object construction/allocation and is not a whole-hospital transaction benchmark.

For one million validations, return-code/exception medians were 0.8057/0.8023 ms at 0% invalid,0.8087/12.1752 ms at 1%, and 4.0447/578.3921 ms at 50%. B2's independent optional comparison also shows substantially higher exception cost on rejection-heavy inputs. Raw trials/checksums are preserved in B2/B3 outputs; generated benchmark_results.md files provide the full data. The zero-error differences are too small to support a universal ranking.

## Decision and limits

Use the polymorphic Bill/Payer interfaces as a maintainable domain boundary, with RAII ownership and typed exceptions for unexpected failures that need cross-layer propagation. The group adds two bill types and three payers each year; this extension demonstrates that such additions can leave existing implementations and processing code unchanged. A few extra dispatch operations do not by themselves justify replacing that boundary with a growing central switch.

For frequent expected business rejection, prefer an explicit result/status/optional-style outcome in a production-facing workflow; reserve exceptions for exceptional failures. The coursework implementation deliberately retains the requested exception experiments and policy behavior. A closed high-volume batch kernel can separately use compact values/variant if profiling real workloads justifies it, without changing the external domain interface.

Treat 200,000 bills/day as the group total, as worded: its arithmetic average is about 2.31 bills/second across 15 hospitals, but bursts, payer integrations, storage and validation work are unspecified. Microbenchmark rates must not be extrapolated into an SLA. Before real deployment, the design needs durable bill/attempt storage, access controls, audited decimal/rounding/business rules, operational monitoring, integration tests and peak-load testing. Clean tests/sanitizers establish only the exercised behavior, not production readiness or real reimbursement correctness.

## Configuration-driven plug-in/rule alternative

```text
LOAD a versioned configuration containing allowed formula kinds and validated rates
REJECT unknown operators, missing fields, negative/nonfinite rates and duplicate codes
BUILD immutable rule objects from the validated configuration
FOR EACH bill request
    LOOK UP the selected bill/payer rule
    EVALUATE only the allowed operations with range checks
    RECORD rule version and the computed decision
END FOR
PUBLISH a new rule set only after tests succeed; retain the previous version for rollback
```

This means data-driven rules, not downloading and executing arbitrary native code. A rule describes only operations supported by its interpreter; genuinely new behavior may still require new C++ code. A fixed native charge performs O(1) arithmetic/dispatch; interpreting a rule of r operations costs O(r), plus lookup, validation and checked arithmetic. Rule storage is O(total rule data), shared across records, whereas native class code/metadata is shared per type and each object stores its own state. Configuration is not automatically faster or safer.

Rate-only changes can avoid recompilation and simplify deployment across 15 hospitals. Conversely, an erroneous configuration can change every hospital's bills at once; versioning, authorization, schema/range checks, test fixtures, staged rollout and rollback are essential. New C++ classes offer compiler-checked interfaces and clear behavior but need builds/deployment. Both approaches require regression tests. A hybrid of stable C++ interfaces with validated, versioned policy data offers a practical extension path; the current demonstration remains explicit C++ classes with no unrequested rule-loader framework.
