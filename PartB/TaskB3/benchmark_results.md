# Actual measured benchmark results

Windows GCC 14.2.0, C++17, -O2. All times below are actual milliseconds. Each group has five trials; medians are derived without discarding observations. See explanation/comparison for scope and limitations.

## benchmark_O2

| Workload | Method | Five actual trials (ms) | Median (ms) |
|---|---|---|---|
| bills=1000, working_set=1000 | virtual | 0.002700, 0.009600, 0.006600, 0.006600, 0.006500 | 0.006600 |
| bills=1000, working_set=1000 | switch | 0.001900, 0.001400, 0.004500, 0.004000, 0.003900 | 0.003900 |
| bills=1000, working_set=1000 | variant_visit | 0.001500, 0.001300, 0.004900, 0.004300, 0.003800 | 0.003800 |
| bills=1000000, working_set=65536 | virtual | 3.265900, 3.831300, 3.316300, 3.582900, 4.826200 | 3.582900 |
| bills=1000000, working_set=65536 | switch | 1.667500, 1.657800, 1.859900, 1.921900, 1.911300 | 1.859900 |
| bills=1000000, working_set=65536 | variant_visit | 1.676500, 1.795200, 1.695700, 1.710600, 1.575500 | 1.695700 |
| bills=10000000, working_set=65536 | virtual | 25.717300, 31.456100, 28.516500, 33.966000, 30.969400 | 30.969400 |
| bills=10000000, working_set=65536 | switch | 16.119800, 17.085400, 16.403300, 16.037300, 16.324100 | 16.324100 |
| bills=10000000, working_set=65536 | variant_visit | 16.477300, 16.001100, 15.461200, 16.345100, 16.189900 | 16.189900 |

## validation_benchmark_O2

| Workload | Method | Five actual trials (ms) | Median (ms) |
|---|---|---|---|
| claims=1000000, invalid_percent=0 | return_code | 0.814400, 0.814700, 0.821700, 0.826100, 0.797600 | 0.814700 |
| claims=1000000, invalid_percent=0 | exceptions | 0.808600, 0.805000, 0.824000, 0.813500, 0.803200 | 0.808600 |
| claims=1000000, invalid_percent=1 | return_code | 0.811200, 0.791100, 0.843900, 0.783200, 0.778100 | 0.791100 |
| claims=1000000, invalid_percent=1 | exceptions | 12.144200, 11.668900, 11.649600, 12.387700, 11.479600 | 11.668900 |
| claims=1000000, invalid_percent=50 | return_code | 4.029800, 3.991700, 3.943000, 4.022900, 3.941400 | 3.991700 |
| claims=1000000, invalid_percent=50 | exceptions | 549.428500, 548.671000, 552.602700, 550.369300, 552.125300 | 550.369300 |

These are measured workload/configuration results, not a universal performance guarantee. Allocation, input generation, logging, error rates, working-set size, compiler decisions, caches and scheduling affect interpretation. Functional checksums/rejection counts and exact build/run commands accompany the raw CSV.
