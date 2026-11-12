# B3 executed comparisons

| ID | Input/scenario | Expected | Actual | Status |
|---|---|---|---|---|
| 1 | Same ten bills, three designs | Per-bill1700/50000/25000/11000/4000/7000/10000/3500/0/4500 | All30 comparisons matched | PASS |
| 2 | Ten-bill sum | 116700 | Independent sum and all three design sums matched | PASS |
| 3 | Replay23 logical bills from ten-record set | Two full sets plus first three records | Three independent checks matched | PASS |
| 4 | Zero logical records | Zero sum | All three returned0 | PASS |
| 5 | Index10, invalid dispatch tag, working sets0/65537, negative tagged charge, unknown input type | Reject appropriately | Six error checks passed | PASS |
| 6 | Fractional0.105 components in each bill family | Per-component paise normalization; totals0.22/0.44/0.11/0.28 | Four cross-design comparisons matched; objects cleaned | PASS |
| 7 | Destruction of owning/variant records | All Bill objects released | Counter returns0 | PASS |
| 8 | 1000/1000000/10000000 logical bills, virtual/switch/visit, five trials | Identical independent checksum in every timed run; payload within budget | All45 checksum checks and three payload guards passed | PASS |
| 9 | End of each benchmark size | No surviving Bill objects | Three cleanup checks passed | PASS |
| 10 | One million claim validations at0/1/50%, return code/exceptions, five trials | Rejections0/10000/500000; approved sums40000000000/39600000000/20000000000 | All30 runs matched;60 count/checksum assertions | PASS |

main records53 runtime evaluations; dispatch benchmark51; validation benchmark60:164 in total. Repeated configurations are not164 distinct input scenarios. Both benchmarks use -O2. Timing values are observations, never pass thresholds; the raw CSV and generated five-trial medians are in outputs/ and benchmark_results.md. Memory-saving scope and exclusions are documented in measurement_plan.md and comparison.md.
