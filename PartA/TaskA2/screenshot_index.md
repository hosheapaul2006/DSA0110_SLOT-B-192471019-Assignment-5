# Genuine Programiz screenshot index

These are original JPEG captures from the real [Programiz C++ compiler](https://www.programiz.com/cpp-programming/online-compiler/). No overlays, regenerated interfaces, image upscaling or edited output were used. Individual captures were visually reviewed. Native dimensions and SHA-256 values are recorded in [the inventory](../../Reference/screenshot_inventory.json).

The native page used its available editor/console font (approximately 14 px); the visible menu offered no font-size control. The requested 16–18 px could not be set through the exposed UI. Open the original image at native scale: do not judge readability from a downscaled GitHub preview. Larger captures include additional source; 1920 × 1080 captures use separate source views where useful.

| Screenshot | Native pixels | Submitted source | What it proves / observed result |
|---|---|---|---|
| [dynamic_cast_alternative_programiz.jpg](screenshots/dynamic_cast_alternative_programiz.jpg) | 1920 × 1080 | [online_dynamic_cast_alternative.cpp](online_dynamic_cast_alternative.cpp) | Driver/end of source and actual output PASS |
| [layout_probe_programiz.jpg](screenshots/layout_probe_programiz.jpg) | 1920 × 1080 | [online_layout_probe.cpp](online_layout_probe.cpp) | Driver/end of source and actual output PASS |
| [predict_output_programiz.jpg](screenshots/predict_output_programiz.jpg) | 1920 × 1080 | [predict_output.cpp](predict_output.cpp) | Driver/end of source and actual output PASS |

Actual console text and source/capture hashes are saved in outputs/*_programiz.txt and outputs/*_programiz.json. The automated comparison checks fixed program text and separately identifies implementation-dependent sizeof values, trailing UI whitespace, and any extra Programiz annotation.
