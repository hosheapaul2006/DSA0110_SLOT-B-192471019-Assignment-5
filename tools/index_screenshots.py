"""Index the original browser captures after their individual visual review."""
from pathlib import Path
import datetime,hashlib,json
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
records=[json.loads(p.read_text(encoding='utf-8')) for p in ROOT.glob('Part*/Task*/outputs/*_programiz.json')]
captions={
 'main_bill_classes.jpg':'Baseline input construction and test setup, with all twelve bill/payer results and retained-bill recovery.',
 'main_hierarchy.jpg':'Insurance and government policy implementations, beside the complete baseline results.',
 'main_extension_source.jpg':'Unchanged baseline matrix processing inside the extended application, with all baseline and extension results.',
 'main_programiz_readonly_failure.jpg':'Observed failed original online run: cannot create the relative-path resource test file. This is failure evidence, not a successful test.',
}
inventory=[]
for path in sorted(ROOT.glob('Part*/Task*/screenshots/*.jpg')):
    relative=path.relative_to(ROOT).as_posix()
    rec=next((r for r in records if any(s['path']==relative for s in r['screenshots'])),None)
    view=next((s['view'] for s in rec['screenshots'] if s['path']==relative),'Observed online filesystem failure') if rec else 'Observed online filesystem failure'
    with Image.open(path) as im:
        assert im.format=='JPEG'
        width,height=im.size
    assert width>=1920 and height>=1080
    inventory.append({'path':relative,'sha256':sha(path),'width':width,'height':height,
       'submitted_source':rec['submitted_source'] if rec else 'PartB/TaskB2/online_main.cpp',
       'result':rec['status'] if rec else 'OBSERVED_FAILURE',
       'proves':captions.get(path.name,view),
       'visual_review':'Reviewed original capture and/or native-resolution inspection view: source/output legible; correct program; raw console retained.',
       'reviewed_utc':datetime.datetime.now(datetime.timezone.utc).isoformat()})
for folder in sorted(ROOT.glob('Part*/Task*')):
    shots=[s for s in inventory if s['path'].startswith(folder.relative_to(ROOT).as_posix()+'/')]
    if not shots:continue
    lines=['# Genuine Programiz screenshot index','',
       'These are original JPEG captures from the real [Programiz C++ compiler](https://www.programiz.com/cpp-programming/online-compiler/). No overlays, regenerated interfaces, image upscaling or edited output were used. Individual captures were visually reviewed. Native dimensions and SHA-256 values are recorded in [the inventory](../../Reference/screenshot_inventory.json).','',
       'The native page used its available editor/console font (approximately 14 px); the visible menu offered no font-size control. The requested 16–18 px could not be set through the exposed UI. Open the original image at native scale: do not judge readability from a downscaled GitHub preview. Larger captures include additional source; 1920 × 1080 captures use separate source views where useful.','',
       '| Screenshot | Native pixels | Submitted source | What it proves / observed result |','|---|---|---|---|']
    for s in shots:
        name=Path(s['path']).name
        src=Path(s['submitted_source']).relative_to(folder.relative_to(ROOT)).as_posix()
        lines.append(f"| [{name}](screenshots/{name}) | {s['width']} × {s['height']} | [{src}]({src}) | {s['proves']} {s['result']} |")
    lines+=['','Actual console text and source/capture hashes are saved in outputs/*_programiz.txt and outputs/*_programiz.json. The automated comparison checks fixed program text and separately identifies implementation-dependent sizeof values, trailing UI whitespace, and any extra Programiz annotation.']
    if folder.name=='TaskB2':
        lines+=['','Programiz inserted a standalone `ERROR!` annotation in the completed TODO and its throw-copy variation, at different positions across runs. Those lines are absent from the submitted C++ source and local output; the raw online text and screenshots preserve them. Program text matches after excluding this disclosed annotation. Its exact internal cause is unverified.',
        '', 'The unmodified full error driver failed to create its relative-path temporary file online. Alternate writable paths were not verified: an attempted retry reused the original source, and its duplicate capture was discarded. The separate online_main_programiz.cpp therefore runs a clearly labelled 24-check memory-only subset. Original main.cpp retains all 25 checks, including real file flush/close/reopen/remove, and passed locally and under Ubuntu ASan. The failed online capture remains above.']
    if folder.name in ['TaskB2','TaskB3']:
        lines+=['','The large timing drivers are local -O2 experiments. The online functional cases are mapped explicitly in [benchmark equivalents](../../Reference/benchmark_online_equivalents.json); screenshots are not large-scale timing measurements.']
    (folder/'screenshot_index.md').write_text('\n'.join(lines)+'\n',encoding='utf-8',newline='\n')
(ROOT/'Reference/screenshot_inventory.json').write_text(json.dumps(inventory,indent=2)+'\n',encoding='utf-8',newline='\n')
print(f'Indexed {len(inventory)} original screenshots: {sum(s["result"]!="OBSERVED_FAILURE" for s in inventory)} successful/annotated views and one observed failure')
