# A1 - Polymorphic hospital bill hierarchy

## Common interface and responsibility

Every Bill has a stable bill ID, a Patient value (record ID and display name), and a package code used by package-based payers. Identity supports traceability; it must not disappear when payment fails. total() supplies the amount, type() labels the category, and print(stream) provides one shared presentation using those virtual operations. Pure virtual total/type make Bill abstract. A public virtual noexcept destructor enables complete destruction through an owning Bill pointer. print itself can remain nonvirtual: calling a nonvirtual wrapper does not suppress the virtual calls inside it.

| Class | Specialized data | total() rule |
|---|---|---|
| OutPatientBill | consultation, tests | consultation + tests |
| InPatientBill | days, room-per-day, procedures, medicines | days*room-per-day + procedures + medicines |
| DayCareBill | package price | package price |
| EmergencyBill | base price, hours, hourly price | base + hours*hourly price |

Patient is composition (has-a), not a bill subtype. All category relationships are public is-a inheritance. Common data is private; derived classes retain only their own charge components. total/type are overridden with override. Constructors reject missing identity, invalid days, nonfinite/negative amounts and unrepresentable totals before a usable Bill exists. A valid zero-priced component is allowed. The demonstration uses double with explicit rounding to the nearest paise and a documented finite numeric range; a real accounting system requires an audited decimal/rounding contract.

Ownership is separate from processing: an array of unique_ptr<Bill> owns the objects, while the required Bill* array provides borrowed views. Payment decisions do not own or delete bills. A rejected claim leaves its bill in the ledger and records an unresolved attempt, permitting a corrected policy or SelfPay retry. The demonstration promises in-process retention, not crash-resistant database durability.

See [UML](diagrams/hierarchy.md) and [structured pseudocode](pseudocode.md). The implemented design and its validation are in [B1](../../PartB/TaskB1/explanation.md).

## Illustrative pre-implementation calculations

These are sample programming inputs, not hospital rates or medical advice.

| Type | Charges in rupees | Hand total |
|---|---|---|
| OP | 500 consultation + 1200 tests | 1700 |
| IP | 3 days*2500 + 40000 procedures + 2500 medicines | 50000 |
| Day care | 25000 package | 25000 |
| Emergency | 3000 base + 4 hours*2000 | 11000 |

Day total: 1700+50000+25000+11000 = 87700. The supplied B1 TODO has no medicines field and intentionally has a different IP total, 47500; its original structure will remain unchanged.

## Type-field/switch alternative

```text
RECORD TaggedBill
    billID, patient, packageCode, kind
    consultation, tests, days, roomPerDay, procedures, medicines
    packagePrice, emergencyBase, hours, hourlyPrice
END RECORD

FUNCTION TOTAL(bill)
    SWITCH bill.kind
        CASE OP: RETURN consultation + tests
        CASE IP: RETURN days*roomPerDay + procedures + medicines
        CASE DAY_CARE: RETURN packagePrice
        CASE EMERGENCY: RETURN emergencyBase + hours*hourlyPrice
        OTHERWISE: RAISE ValidationError
    END SWITCH
END FUNCTION
```

Both designs process n bills in O(n) calls for this fixed set of categories, with O(1) charge arithmetic per bill. Fixed control fields occupy O(1) per object, plus O(patient/package text length) for strings. A tagged record reserves irrelevant charge fields; specialized classes avoid those fields but commonly add a vptr and alignment padding, plus a pointer in a heterogeneous processing array. sizeof excludes string heap allocations and allocator metadata. Actual measurements belong to B3; neither a memory saving nor a speed advantage is assumed here.

For a new category, the tagged design normally changes the kind enum, total switch and construction/registration code (three sites), plus the record if new fields are needed. The hierarchy adds one derived class and changes application construction (two sites), leaving the existing class implementations and processing loop untouched. A closed, small set can favor the simpler switch; frequent independent extensions favor the shared interface.
