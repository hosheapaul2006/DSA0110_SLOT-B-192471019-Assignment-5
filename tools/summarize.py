"""Derive task-local statistics only from saved actual C++ CSV timings."""
import csv
import json
from pathlib import Path
import statistics

ROOT=Path(__file__).resolve().parents[1]
all_summaries={}
for folder in sorted((ROOT/'PartB').glob('TaskB*')):
    files=sorted((folder/'outputs').glob('*benchmark_O2_actual.txt'))
    if not files:continue
    lines=['# Actual measured benchmark results','','Windows GCC 14.2.0, C++17, -O2. All times below are actual milliseconds. Each group has five trials; medians are derived without discarding observations. See explanation/comparison for scope and limitations.','']
    for path in files:
        rows=list(csv.DictReader(path.read_text().splitlines()))
        groups={}
        for row in rows:
            assert float(row['elapsed_ms'])>=0
            identity=tuple((k,row[k]) for k in ['method','claims','invalid_percent','bills','working_set'] if k in row)
            groups.setdefault(identity,[]).append(row)
        lines+=['## '+path.stem.removesuffix('_actual'),'','| Workload | Method | Five actual trials (ms) | Median (ms) |','|---|---|---|---|']
        for identity,group in groups.items():
            assert sorted(int(row['trial']) for row in group)==[1,2,3,4,5]
            values=[float(row['elapsed_ms']) for row in sorted(group,key=lambda r:int(r['trial']))]
            fields=dict(identity);method=fields.pop('method')
            workload=', '.join(k+'='+v for k,v in fields.items())
            median=statistics.median(values)
            key=folder.relative_to(ROOT).as_posix()+'/'+path.stem.removesuffix('_actual')+'/'+workload+'/'+method
            all_summaries[key]={'trials_ms':values,'median_ms':median,'workload':fields,'method':method,
                                'source_csv':path.relative_to(ROOT).as_posix()}
            lines.append('| '+workload+' | '+method+' | '+', '.join(f'{v:.6f}' for v in values)+f' | {median:.6f} |')
        lines+=['']
    lines+=['These are measured workload/configuration results, not a universal performance guarantee. Allocation, input generation, logging, error rates, working-set size, compiler decisions, caches and scheduling affect interpretation. Functional checksums/rejection counts and exact build/run commands accompany the raw CSV.']
    (folder/'benchmark_results.md').write_text('\n'.join(lines)+'\n',encoding='utf-8',newline='\n')
(ROOT/'Reference/benchmark_summary.json').write_text(json.dumps(all_summaries,indent=2)+'\n',encoding='utf-8',newline='\n')
for name,value in all_summaries.items():print(name, value['median_ms'],'ms')
