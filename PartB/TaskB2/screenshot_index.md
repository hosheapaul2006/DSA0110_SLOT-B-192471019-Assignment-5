# Genuine Programiz screenshot index

These are original JPEG captures from the real [Programiz C++ compiler](https://www.programiz.com/cpp-programming/online-compiler/). No overlays, regenerated interfaces, image upscaling or edited output were used. Individual captures were visually reviewed. Native dimensions and SHA-256 values are recorded in [the inventory](../../Reference/screenshot_inventory.json).

The native page used its available editor/console font (approximately 14 px); the visible menu offered no font-size control. The requested 16–18 px could not be set through the exposed UI. Open the original image at native scale: do not judge readability from a downscaled GitHub preview. Larger captures include additional source; 1920 × 1080 captures use separate source views where useful.

| Screenshot | Native pixels | Submitted source | What it proves / observed result |
|---|---|---|---|
| [complete_code_programiz.jpg](screenshots/complete_code_programiz.jpg) | 1920 × 1080 | [complete_code.cpp](complete_code.cpp) | Completed TODO source and handled failures; Programiz ERROR! annotation preserved MATCH_WITH_ADDITIONAL_PROGRAMIZ_MARKER |
| [experiments_rethrow_types_programiz.jpg](screenshots/experiments_rethrow_types_programiz.jpg) | 2649 × 2788 | [online_experiments_rethrow_types.cpp](online_experiments_rethrow_types.cpp) | Source driver and actual output PASS |
| [experiments_throw_copy_exact_programiz.jpg](screenshots/experiments_throw_copy_exact_programiz.jpg) | 2649 × 2788 | [experiments/throw_copy_exact.cpp](experiments/throw_copy_exact.cpp) | Source driver and actual output MATCH_WITH_ADDITIONAL_PROGRAMIZ_MARKER |
| [functional_validation_programiz.jpg](screenshots/functional_validation_programiz.jpg) | 2649 × 2788 | [online_functional_validation.cpp](online_functional_validation.cpp) | Source driver and actual output PASS |
| [main_programiz.jpg](screenshots/main_programiz.jpg) | 1920 × 1080 | [online_main_programiz.cpp](online_main_programiz.cpp) | 24-check memory-only online subset; full file test remains local/Ubuntu-only PASS |
| [main_programiz_readonly_failure.jpg](screenshots/main_programiz_readonly_failure.jpg) | 2649 × 2788 | [online_main.cpp](online_main.cpp) | Observed failed original online run: cannot create the relative-path resource test file. This is failure evidence, not a successful test. OBSERVED_FAILURE |
| [predict_output_programiz.jpg](screenshots/predict_output_programiz.jpg) | 2649 × 2788 | [predict_output.cpp](predict_output.cpp) | Source driver and actual output PASS |

Actual console text and source/capture hashes are saved in outputs/*_programiz.txt and outputs/*_programiz.json. The automated comparison checks fixed program text and separately identifies implementation-dependent sizeof values, trailing UI whitespace, and any extra Programiz annotation.

Programiz inserted a standalone `ERROR!` annotation in the completed TODO and its throw-copy variation, at different positions across runs. Those lines are absent from the submitted C++ source and local output; the raw online text and screenshots preserve them. Program text matches after excluding this disclosed annotation. Its exact internal cause is unverified.

The unmodified full error driver failed to create its relative-path temporary file online. Alternate writable paths were not verified: an attempted retry reused the original source, and its duplicate capture was discarded. The separate online_main_programiz.cpp therefore runs a clearly labelled 24-check memory-only subset. Original main.cpp retains all 25 checks, including real file flush/close/reopen/remove, and passed locally and under Ubuntu ASan. The failed online capture remains above.

The large timing drivers are local -O2 experiments. The online functional cases are mapped explicitly in [benchmark equivalents](../../Reference/benchmark_online_equivalents.json); screenshots are not large-scale timing measurements.
