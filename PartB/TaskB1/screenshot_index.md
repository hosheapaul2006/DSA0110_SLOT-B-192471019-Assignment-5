# Genuine Programiz screenshot index

These are original JPEG captures from the real [Programiz C++ compiler](https://www.programiz.com/cpp-programming/online-compiler/). No overlays, regenerated interfaces, image upscaling or edited output were used. Individual captures were visually reviewed. Native dimensions and SHA-256 values are recorded in [the inventory](../../Reference/screenshot_inventory.json).

The native page used its available editor/console font (approximately 14 px); the visible menu offered no font-size control. The requested 16–18 px could not be set through the exposed UI. Open the original image at native scale: do not judge readability from a downscaled GitHub preview. Larger captures include additional source; 1920 × 1080 captures use separate source views where useful.

| Screenshot | Native pixels | Submitted source | What it proves / observed result |
|---|---|---|---|
| [complete_code_programiz.jpg](screenshots/complete_code_programiz.jpg) | 1920 × 1080 | [complete_code.cpp](complete_code.cpp) | Driver/end of source and actual output PASS |
| [experiments_inherited_virtual_omission_programiz.jpg](screenshots/experiments_inherited_virtual_omission_programiz.jpg) | 1920 × 1080 | [online_experiments_inherited_virtual_omission.cpp](online_experiments_inherited_virtual_omission.cpp) | Driver/end of source and actual output PASS |
| [experiments_nonvirtual_binding_programiz.jpg](screenshots/experiments_nonvirtual_binding_programiz.jpg) | 1920 × 1080 | [experiments/nonvirtual_binding.cpp](experiments/nonvirtual_binding.cpp) | Driver/end of source and actual output PASS |
| [experiments_nonvirtual_binding_source_top.jpg](screenshots/experiments_nonvirtual_binding_source_top.jpg) | 1920 × 1080 | [experiments/nonvirtual_binding.cpp](experiments/nonvirtual_binding.cpp) | Base total is genuinely nonvirtual; the actual zero totals remain visible PASS |
| [main_bill_classes.jpg](screenshots/main_bill_classes.jpg) | 1920 × 1080 | [online_main.cpp](online_main.cpp) | Baseline input construction and test setup, with all twelve bill/payer results and retained-bill recovery. PASS |
| [main_hierarchy.jpg](screenshots/main_hierarchy.jpg) | 1920 × 1080 | [online_main.cpp](online_main.cpp) | Insurance and government policy implementations, beside the complete baseline results. PASS |
| [main_programiz.jpg](screenshots/main_programiz.jpg) | 1920 × 1080 | [online_main.cpp](online_main.cpp) | Source driver and actual output PASS |
| [payer_alternatives_programiz.jpg](screenshots/payer_alternatives_programiz.jpg) | 1920 × 1080 | [online_payer_alternatives.cpp](online_payer_alternatives.cpp) | Source driver and actual output PASS |
| [template_alternative_programiz.jpg](screenshots/template_alternative_programiz.jpg) | 1920 × 1080 | [online_template_alternative.cpp](online_template_alternative.cpp) | Source driver and actual output PASS |

Actual console text and source/capture hashes are saved in outputs/*_programiz.txt and outputs/*_programiz.json. The automated comparison checks fixed program text and separately identifies implementation-dependent sizeof values, trailing UI whitespace, and any extra Programiz annotation.
