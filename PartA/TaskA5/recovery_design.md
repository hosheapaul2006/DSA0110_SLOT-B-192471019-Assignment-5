# A5 - Catch, rethrow and recover

## Boundaries and ownership

1. Parse and validate a request before committing a new valid bill. A failed request is still present in its input fixture/request queue; it is not turned into a zero-price bill.
2. The ledger owns each valid Bill through unique_ptr independently of settlement. A payment policy borrows const Bill&, so a claim error cannot remove or mutate its charge components.
3. The settlement service catches BillingError by const reference, logs context best-effort, then uses `throw;`. Logging is not recovery. A failed log must not replace the primary exception, so the logging helper catches its own failures.
4. The per-request application boundary catches specific errors first and std::exception next. It records/reports rejection while keeping the bill pending. A catch-all handles a deliberately unknown thrown type, releases local resources through unwinding and continues with the next independent request.
5. Genuine recovery requires a changed, valid action: correct the request/policy and retry, or use an explicitly selected alternative payer. Merely printing an error and retrying identical invalid input forever is not recovery. The sample will demonstrate a rejected insured claim followed by a successful SelfPay retry on the same owned bill.
6. Real allocation failure may make ordinary logging/retry impossible. Use a small fixed diagnostic, release scoped resources, and fail the current operation safely. The tests inject a narrow failure and do not claim to reproduce real OS memory exhaustion.

The demonstration retains bills in process memory; it is not a crash-safe database. A production deployment needs durable transaction/attempt records, audit requirements and authorization around changes of payer. The sample does not make medical or real insurance decisions.

## Resource safety

Stack unwinding destroys fully constructed automatic objects in reverse construction order. If a constructor throws, that object's destructor does not run, but its completed bases/members are destroyed. unique_ptr releases a complete dynamically allocated object; virtual destruction is still required when the owner type is a polymorphic base. vector/string own their storage; fstream objects close file handles in destruction. Explicit checked flush/close operations are needed when close/write failure itself must be reported; destructors remain noexcept and perform best-effort cleanup.

The supplied Guard experiment predicted `~f ~main caught 42` followed by newline. f's guard is destroyed before main's try-scope guard; then the int handler runs. The statement after f() is unreachable because the call throws rather than returns normally. The catch-all does not run for that int. The exact comparison matched, with the original prediction committed in 9000c57 and unchanged afterward. B2 will add controlled allocation, file and catch-all execution tests; those are not claimed as already executed here.

## Manual cleanup alternative

```text
resource <- new Resource
TRY
    process(resource)
CATCH ClaimError
    delete resource
    report claim error
CATCH PackageError
    delete resource
    report package error
CATCH all other failures
    delete resource
    propagate
END TRY
delete resource          // success path needs cleanup too
```

This spelling has four cleanup sites for one allocation: three error paths and success. Omitting the catch-all leaks on a newly introduced error; adding a new distinct handler introduces another place where deletion may be forgotten. Throwing while reporting or inserting an early return adds more hazards, and an exception between multiple raw allocations can leak earlier ones before the try begins. Deleting twice can double-free. RAII registers ownership once, and all exits use the same destructor path, avoiding duplicated delete logic.

Both approaches ultimately perform the same resource release work. With r resources and h unwound scopes, cleanup is proportional to the resources/destructors crossed, not magically constant time. RAII owner wrappers usually add only a pointer for a default-deleter unique_ptr, but the exact layout is implementation-specific. Manual repeated handlers increase source/code-maintenance burden rather than improving Big-O. Owning a resource through a raw pointer does not provide automatic cleanup merely because a catch block exists.

## Rethrow semantics

`throw;` rethrows the currently handled exception object and preserves its dynamic type. `throw e;` throws a copied expression of e's static type. Catching a ClaimError and throwing that same ClaimError type creates a copy but need not change the dynamic type. Catching a more-derived object through a base reference and throwing that reference expression can slice it. B2 will execute both situations explicitly rather than incorrectly claiming that every `throw e;` changes type.
