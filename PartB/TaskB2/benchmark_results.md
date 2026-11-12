# Actual measured benchmark results

Windows GCC 14.2.0, C++17, -O2. All times below are actual milliseconds. Each group has five trials; medians are derived without discarding observations. See explanation/comparison for scope and limitations.

## benchmark_O2

| Workload | Method | Five actual trials (ms) | Median (ms) |
|---|---|---|---|
| claims=1000000, invalid_percent=0 | optional | 0.823500, 0.809800, 0.809300, 0.822600, 0.830700 | 0.822600 |
| claims=1000000, invalid_percent=0 | exceptions | 0.811400, 0.803500, 0.814500, 0.801800, 0.806500 | 0.806500 |
| claims=1000000, invalid_percent=1 | optional | 0.819000, 0.787000, 0.758300, 0.790300, 0.749300 | 0.787000 |
| claims=1000000, invalid_percent=1 | exceptions | 12.193400, 11.956400, 11.576000, 11.538200, 14.223700 | 11.956400 |
| claims=1000000, invalid_percent=50 | optional | 3.737800, 3.761600, 3.750700, 3.869200, 3.874300 | 3.761600 |
| claims=1000000, invalid_percent=50 | exceptions | 553.897700, 556.484800, 551.797100, 545.792000, 544.210100 | 551.797100 |

These are measured workload/configuration results, not a universal performance guarantee. Allocation, input generation, logging, error rates, working-set size, compiler decisions, caches and scheduling affect interpretation. Functional checksums/rejection counts and exact build/run commands accompany the raw CSV.
