# A1 - Total the day's bills

Conventional structured pseudocode is used with the user's approval. The named official SIMATS formatting guide was not supplied or located; official-format compliance is not claimed. This pseudocode is a required design artifact, not replaced by C++.

```text
ALGORITHM DAILY_TOTAL
INPUT: bills, an array of borrowed Bill pointers; count
OUTPUT: validated total OR a clearly reported failure
BEGIN
    REQUIRE count >= 0
    REQUIRE bills exists when count > 0
    sum <- 0
    FOR index <- 0 TO count - 1
        IF bills[index] is null THEN
            RAISE ValidationError("missing bill pointer")
        END IF
        amount <- bills[index].total()   // dynamic dispatch; no type tests
        CHECK amount and sum+amount are representable monetary amounts
        sum <- sum + amount
    END FOR
    RETURN sum
END
```

The owner collection remains alive while views are used. A failure does not delete or remove any bill. An empty array totals zero. A partially accumulated amount is not reported as the complete day total after a failure. Processing is O(n) for bounded-size values; auxiliary accumulation space is O(1), excluding the existing O(n) bill/pointer storage.
