# Actual measured benchmark results

Windows GCC 14.2.0, C++17, -O2. All times below are actual milliseconds. Each group has five trials; medians are derived without discarding observations. See explanation/comparison for scope and limitations.

## benchmark_O2

| Workload | Method | Five actual trials (ms) | Median (ms) |
|---|---|---|---|
| claims=1000000, invalid_percent=0 | optional | 0.806700, 0.793100, 0.835100, 0.834800, 0.831900 | 0.831900 |
| claims=1000000, invalid_percent=0 | exceptions | 0.806800, 0.877600, 0.859300, 0.829500, 0.832200 | 0.832200 |
| claims=1000000, invalid_percent=1 | optional | 0.812900, 0.831800, 0.810000, 0.823700, 0.816400 | 0.816400 |
| claims=1000000, invalid_percent=1 | exceptions | 12.878300, 15.857900, 13.769900, 13.540800, 13.197500 | 13.540800 |
| claims=1000000, invalid_percent=50 | optional | 4.144900, 3.942500, 3.904400, 3.991400, 4.368000 | 3.991400 |
| claims=1000000, invalid_percent=50 | exceptions | 588.653300, 562.409400, 598.861800, 572.106900, 621.505600 | 588.653300 |

These are measured workload/configuration results, not a universal performance guarantee. Allocation, input generation, logging, error rates, working-set size, compiler decisions, caches and scheduling affect interpretation. Functional checksums/rejection counts and exact build/run commands accompany the raw CSV.
