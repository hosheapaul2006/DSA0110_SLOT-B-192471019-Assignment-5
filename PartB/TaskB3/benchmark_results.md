# Actual measured benchmark results

Windows GCC 14.2.0, C++17, -O2. All times below are actual milliseconds. Each group has five trials; medians are derived without discarding observations. See explanation/comparison for scope and limitations.

## benchmark_O2

| Workload | Method | Five actual trials (ms) | Median (ms) |
|---|---|---|---|
| bills=1000, working_set=1000 | virtual | 0.002600, 0.002400, 0.002100, 0.002100, 0.002200 | 0.002200 |
| bills=1000, working_set=1000 | switch | 0.001600, 0.001400, 0.001400, 0.001200, 0.001400 | 0.001400 |
| bills=1000, working_set=1000 | variant_visit | 0.002100, 0.001400, 0.001300, 0.002300, 0.001300 | 0.001400 |
| bills=1000000, working_set=65536 | virtual | 3.158900, 4.548200, 3.560400, 3.838700, 4.001100 | 3.838700 |
| bills=1000000, working_set=65536 | switch | 2.064700, 2.008600, 1.926300, 1.732700, 2.203500 | 2.008600 |
| bills=1000000, working_set=65536 | variant_visit | 1.629200, 1.748000, 1.753900, 1.742900, 1.742300 | 1.742900 |
| bills=10000000, working_set=65536 | virtual | 26.894400, 35.697700, 29.884900, 27.312500, 32.374200 | 29.884900 |
| bills=10000000, working_set=65536 | switch | 23.142300, 27.303000, 18.574600, 21.810800, 21.388700 | 21.810800 |
| bills=10000000, working_set=65536 | variant_visit | 16.817700, 18.209800, 15.879400, 17.823100, 17.691300 | 17.691300 |

## validation_benchmark_O2

| Workload | Method | Five actual trials (ms) | Median (ms) |
|---|---|---|---|
| claims=1000000, invalid_percent=0 | return_code | 0.805700, 0.815700, 0.792500, 0.797600, 0.844600 | 0.805700 |
| claims=1000000, invalid_percent=0 | exceptions | 0.793500, 0.805200, 0.797400, 0.802300, 0.807300 | 0.802300 |
| claims=1000000, invalid_percent=1 | return_code | 0.787800, 0.808700, 0.805200, 0.829300, 0.815800 | 0.808700 |
| claims=1000000, invalid_percent=1 | exceptions | 12.540000, 11.999700, 12.237500, 12.112500, 12.175200 | 12.175200 |
| claims=1000000, invalid_percent=50 | return_code | 4.044700, 4.108900, 4.015800, 4.117000, 3.931900 | 4.044700 |
| claims=1000000, invalid_percent=50 | exceptions | 585.272900, 586.121400, 575.717000, 570.992000, 578.392100 | 578.392100 |

These are measured workload/configuration results, not a universal performance guarantee. Allocation, input generation, logging, error rates, working-set size, compiler decisions, caches and scheduling affect interpretation. Functional checksums/rejection counts and exact build/run commands accompany the raw CSV.
