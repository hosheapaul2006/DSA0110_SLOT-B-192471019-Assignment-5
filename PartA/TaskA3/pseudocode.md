# A3 - Policy dispatch and extension

Conventional structured pseudocode is used under the user's approved fallback; the official SIMATS formatting guide was not supplied or located.

```text
ABSTRACT CLASS Payer
    ABSTRACT FUNCTION amountCovered(bill)
    VIRTUAL NONTHROWING DESTRUCTOR
END CLASS

CLASS SelfPay EXTENDS Payer
    FUNCTION amountCovered(bill): RETURN 0
END CLASS

CLASS Insurance EXTENDS Payer
    DATA coverLimit, coPaymentPercent
    FUNCTION amountCovered(bill)
        proposed <- ROUND_TO_PAISE(bill.total()*(100-coPaymentPercent)/100)
        IF proposed > coverLimit THEN RAISE ClaimError("exceeds cover")
        RETURN proposed
    END FUNCTION
END CLASS

CLASS GovernmentScheme EXTENDS Payer
    DATA validatedPackageTable
    FUNCTION amountCovered(bill)
        IF bill.packageCode not in table THEN RAISE PackageError("unknown package")
        RETURN MIN(bill.total(), table[bill.packageCode])
    END FUNCTION
END CLASS

CLASS CorporatePayer EXTENDS Payer   // later extension; existing classes untouched
    DATA coverageFraction, cap
    FUNCTION amountCovered(bill)
        RETURN MIN(ROUND_TO_PAISE(bill.total()*coverageFraction), cap)
    END FUNCTION
END CLASS

FUNCTION SETTLE(bill, payer)
    covered <- payer.amountCovered(bill)
    REQUIRE covered is finite AND 0 <= covered <= bill.total()
    RETURN approved result(total, covered, total-covered)
END FUNCTION
```

Rejections propagate to the recovery boundary rather than silently returning an approved result. Bills remain owned independently of settlement attempts.
