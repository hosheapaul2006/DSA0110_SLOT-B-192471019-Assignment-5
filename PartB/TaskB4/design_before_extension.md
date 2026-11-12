# B4 extension contract recorded before new classes

Add DiagnosticBill directly under Bill with scanFee/reportFee fields, total/type overrides and inherited virtual destruction. Sample scan6000+report500 gives6500. Add CorporatePayer directly under Payer, covering80% capped at100000. Expected coverage5200 and patient1300 for that diagnostic bill. A pre-existing DayCareBill of250000 should receive the corporate cap100000, leaving150000 for the patient. Existing Insurance(50000,20) should also cover5200 on the new bill, demonstrating both old-payer/new-bill and new-payer/old-bill integration.

Existing government packages do not include DX_STD. Do not invent a new allowance: report PackageError and retain the bill; an explicitly chosen valid payer can settle it later. Invalid extension charges/policy data must fail safely. Test thrown/caught ClaimError, PackageError and injected bad_alloc while owners remain valid, then prove all objects are released.

Before adding code, snapshot every existing PartA/PartB .cpp/.hpp plus tools/checks.hpp in Reference/baseline_before_B4.json. New classes go only in extensions.hpp. runBillingTests from B1 remains unchanged and is called by the combined main before the new extension suite. The extension-only driver independently demonstrates the new behavior. After implementation, compare all protected source hashes and rerun earlier safe programs, including the benchmark checks.

No existing bill, payer, exception, test or experiment class needs a new method, switch case or source edit. Application construction/new configuration is allowed; this extension does not alter the existing government allowance table or the closed B3 benchmark's type list.
