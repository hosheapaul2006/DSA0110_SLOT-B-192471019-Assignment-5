# A4 - Exception strategy

## Error taxonomy

| Error | Detection point | Exception/result | Ownership/recovery |
|---|---|---|---|
| Negative/nonfinite charge, invalid days, missing identity, invalid percentage | Constructors and input validation | ValidationError | Do not publish a partially valid object; retain the input request |
| Proposed insurance contribution exceeds cover | Insurance policy | ClaimError("exceeds cover") | Keep bill, record rejected attempt; correction or another approved payer can be tried |
| Unknown package code | GovernmentScheme lookup | PackageError | Keep bill; correct configuration/code, never invent a package allowance |
| Patient record ID absent | PatientRegistry lookup before bill construction | MissingPatientError | Report the missing record; keep existing bills/inputs unchanged |
| Invalid numeric text, e.g. abc | std::stod | std::invalid_argument | Ask for/retry valid input; no partial financial state update |
| Numeric text outside double range | std::stod | std::out_of_range | Report range problem; do not substitute zero |
| Allocation cannot be satisfied | Standard allocation or labelled failure injector | std::bad_alloc | RAII cleanup; minimal reporting; do not assume real exhaustion is recoverable |
| Unknown thrown type | Outer per-request boundary | catch(...) | Report unknown failure; continue with the next independent request after cleanup |

BillingError derives from std::runtime_error, which derives from std::exception. ValidationError, ClaimError and PackageError derive from BillingError; MissingPatientError derives from ValidationError. Standard conversion/allocation exceptions keep their standard types and are caught through std::exception where appropriate. This avoids a misleading claim that a custom ValidationError is automatically std::invalid_argument.

## Exception contracts

| Operation | Contract and reason |
|---|---|
| Patient/Bill/policy constructors | May throw validation, allocation and standard string/container exceptions |
| Bill::total() | Planned noexcept after constructors validate all immutable components and the full bounded calculation; produces normalized paise result |
| Bill::type(), ID accessors, Payer::name() | noexcept: return stored primitive/borrowed fixed data, allocate nothing |
| Bill::print(stream) | May throw stream failures; no noexcept promise for caller-controlled streams |
| Payer::amountCovered | May throw claim/package/validation errors; total's noexcept does not make policy lookup nonthrowing |
| Numeric parsing, patient/package lookup | May throw documented standard/custom exceptions |
| Settlement wrapper | Logs a domain failure and uses bare throw; it does not suppress failure or claim recovery |
| Bill/Payer/resource destructors | noexcept; release owned resources and never propagate errors |
| Diagnostics used during cleanup | Best effort; catch internal failures, avoid throwing from a destructor |

The demonstration will normalize monetary components to the nearest paise, reject nonfinite/negative or above-1e12-rupee values, and validate derived full totals before publishing objects. total() can then repeat the same bounded calculation without allocation or validation exceptions. Inpatient multiplication and emergency time charges need validation before a noexcept accessor is promised. Derived overrides must honor the base exception specification. If an implementation later needs a throwing total(), the design/contract must be changed explicitly rather than allowing terminate accidentally.

C++17 uses noexcept and ordinary documented throws; deprecated/removed dynamic exception lists such as throw(ClaimError) are not appropriate. noexcept does not catch exceptions: if one escapes, std::terminate is invoked. Destructors should not throw, especially while an earlier exception is unwinding the stack; a second escaping exception can terminate the process and prevent ordinary recovery. Close/write failures that matter must be handled by an explicit checked operation before destruction, while the destructor still performs nonthrowing cleanup.

## Status-code alternative

```text
ENUM Status: Ok, InvalidCharge, AboveCover, UnknownPackage, MissingPatient, AllocationFailed
RECORD Result: status, paidAmount, errorCode

FUNCTION APPROVE_WITH_STATUS(amount, cover)
    IF amount < 0 OR cover < 0 THEN RETURN Result(InvalidCharge, 0)
    IF amount > cover THEN RETURN Result(AboveCover, 0)
    RETURN Result(Ok, amount)
END FUNCTION

FUNCTION SERVICE(request)
    result <- APPROVE_WITH_STATUS(request.amount, request.cover)
    IF result.status != Ok THEN RETURN result
    // Every caller must inspect/forward status before using the amount.
    RETURN result
END FUNCTION
```

Both mechanisms still need validation branches. Normal-path validation over n records is O(n); status/optional carries a tag/value and explicit checks through callers, while exceptions use implementation-specific unwind metadata and can affect optimization/code size. Exceptions are not guaranteed cost-free in normal execution. With r errors crossing h frames, exceptional propagation includes unwinding and destructors, modeled as O(r*h + cleanup work), while status propagation requires checks/returns through those frames. A message can allocate in either design. Catching a normal business rejection millions of times can be expensive; B2/B3 will measure rather than invent that cost. Return codes/optional suit expected rejection-heavy workflows; exceptions suit failures that cannot be handled at the point of discovery. A bool alone discards the reason unless supplemented with an error code/context.

Allocation tests will use a clearly labelled injected bad_alloc at a narrow allocation point. This tests cleanup/handling, not actual OS memory pressure. It avoids overcommit ambiguity and deliberate exhaustion of the user's machine. Real bad_alloc should not trigger allocation-heavy recovery promises.
