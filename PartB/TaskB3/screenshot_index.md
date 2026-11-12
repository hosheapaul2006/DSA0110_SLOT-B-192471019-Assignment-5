# Genuine Programiz screenshot index

These are original JPEG captures from the real [Programiz C++ compiler](https://www.programiz.com/cpp-programming/online-compiler/). No overlays, regenerated interfaces, image upscaling or edited output were used. Individual captures were visually reviewed. Native dimensions and SHA-256 values are recorded in [the inventory](../../Reference/screenshot_inventory.json).

The native page used its available editor/console font (approximately 14 px); the visible menu offered no font-size control. The requested 16–18 px could not be set through the exposed UI. Open the original image at native scale: do not judge readability from a downscaled GitHub preview. Larger captures include additional source; 1920 × 1080 captures use separate source views where useful.

| Screenshot | Native pixels | Submitted source | What it proves / observed result |
|---|---|---|---|
| [main_programiz.jpg](screenshots/main_programiz.jpg) | 2649 × 2788 | [online_main.cpp](online_main.cpp) | Source driver and actual output PASS |

Actual console text and source/capture hashes are saved in outputs/*_programiz.txt and outputs/*_programiz.json. The automated comparison checks fixed program text and separately identifies implementation-dependent sizeof values, trailing UI whitespace, and any extra Programiz annotation.

The large timing drivers are local -O2 experiments. The online functional cases are mapped explicitly in [benchmark equivalents](../../Reference/benchmark_online_equivalents.json); screenshots are not large-scale timing measurements.
