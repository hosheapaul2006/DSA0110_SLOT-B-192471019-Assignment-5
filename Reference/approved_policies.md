# User-confirmed Assignment 5 policies

The user accepted all three recommendations before implementation.

- Insurance cover 50000 rupees, patient co-payment 20%. Proposed insurer share is 80% of total; reject the claim with ClaimError if that share exceeds cover. Retain the bill for correction/recovery. SelfPay covers zero. The supplied TODO approve(billed,cover) keeps its own explicit billed-versus-cover rule.
- Government sample package allowances: OP 1500, IP 40000, day-care 20000, emergency 10000. Pay min(total,allowance); unknown package codes are rejected.
- Later extension: DiagnosticBill is scan 6000 plus reporting 500; CorporatePayer pays 80% capped at 100000. These are illustrative programming policies, not real reimbursement advice or prices.
- Conventional structured pseudocode is approved because the named official SIMATS formatting guide was not supplied or located. No official-format compliance claim.
- Safe labelled allocation-failure injection, bounded benchmark batches, real Ubuntu sanitizer execution, no report. Local GCC 14.2.0 lacks ASan/LSan runtimes.
- Benchmarks may process 10M logical bills in 65536-object batches. Initial read-only memory observation was about 1.244 GiB available/91% load; never retain 10M full billing objects or attempt real memory exhaustion. Exact timing scopes and actual memory sizes must be recorded.
- The user will submit the GitHub link personally. Keep the existing repository name and flag its discrepancy with the PDF convention.
