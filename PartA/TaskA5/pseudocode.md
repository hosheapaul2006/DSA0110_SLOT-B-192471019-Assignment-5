# A5 - Recovery algorithm

Conventional structured pseudocode is the user-approved fallback; the official SIMATS guide was unavailable.

```text
PROCEDURE SETTLE_WITH_AUDIT(bill, payer)
    TRY
        RETURN payer.amountCovered(bill)
    CATCH BillingError AS error
        TRY
            WRITE best-effort audit(bill.id, payer.name, error.message)
        CATCH logging failure
            // Keep the original billing error active.
        END TRY
        THROW                 // bare rethrow; no successful result
    END TRY
END PROCEDURE

PROCEDURE PROCESS_REQUESTS(ledger, requests)
    FOR EACH request IN requests
        TRY
            bill <- ledger.find(request.billID)  // borrowed; ledger still owns bill
            covered <- SETTLE_WITH_AUDIT(bill, request.payer)
            CHECK 0 <= covered <= bill.total()
            REPORT approved(total, covered, total-covered)
        CATCH ClaimError OR PackageError
            KEEP bill pending
            REPORT reason and allow an explicitly corrected request
        CATCH bad_alloc
            RELEASE scoped resources through RAII
            REPORT minimal allocation-failure diagnostic
        CATCH other standard exception
            KEEP existing ledger bills unchanged
            REPORT error
        CATCH all other failures
            KEEP existing ledger bills unchanged
            REPORT unknown failure
        END TRY
    END FOR
END PROCEDURE
```

Local owners/file handles are created immediately in RAII wrappers; no duplicated delete statements are needed in handlers. The loop proceeds to the next independent request after recovery/reporting, rather than pretending the failed request succeeded.
