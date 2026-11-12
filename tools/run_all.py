"""Reproduce safe executions, isolated-only compilation and real diagnostics."""
import json
import re
from pathlib import Path
from verify import ROOT,verify,flatten

ERRORS={
 'PartA/TaskA2/experiments/destructor_keyword_only.cpp':'does not override',
 'PartB/TaskB1/experiments/root_pure_without_virtual.cpp':'non-virtual',
 'PartB/TaskB1/experiments/nonvirtual_override_error.cpp':'does not override',
}
UNSAFE={
 'PartA/TaskA2/experiments/nonvirtual_destructor.cpp',
 'PartB/TaskB1/experiments/nonvirtual_destructor.cpp',
}
EXPECTED={
 'PartA/TaskA2/predict_output.cpp':'outputs/predict_output_predicted.txt',
 'PartA/TaskA5/predict_output.cpp':'outputs/predict_output_predicted.txt',
 'PartB/TaskB1/complete_code.cpp':'outputs/complete_expected.txt',
 'PartB/TaskB1/experiments/nonvirtual_binding.cpp':'outputs/nonvirtual_expected.txt',
 'PartB/TaskB2/predict_output.cpp':'outputs/predict_output_predicted.txt',
 'PartB/TaskB2/complete_code.cpp':'outputs/complete_expected.txt',
 'PartB/TaskB2/experiments/throw_copy_exact.cpp':'outputs/complete_expected.txt',
}
BENCHMARK_EQUIVALENTS={
 'PartB/TaskB2/benchmark.cpp':'PartB/TaskB2/functional_validation.cpp',
 'PartB/TaskB3/benchmark.cpp':'PartB/TaskB3/main.cpp',
 'PartB/TaskB3/validation_benchmark.cpp':'PartB/TaskB2/functional_validation.cpp',
}

def main():
    manifest=[];records=[]
    sources=sorted(p for part in ['PartA','PartB'] for p in (ROOT/part).rglob('*.cpp') if not p.name.startswith('online_'))
    for path in sources:
        relative=path.relative_to(ROOT).as_posix()
        parts=Path(relative).parts;task='/'.join(parts[:2]);source='/'.join(parts[2:])
        if relative in ERRORS:
            records.append(verify(task,source,diagnostic='error',needle=ERRORS[relative]));continue
        if relative in UNSAFE:
            records.append(verify(task,source,diagnostic='warning',needle='non-virtual',compile_only=True));continue
        if relative in BENCHMARK_EQUIVALENTS:
            records.append(verify(task,source,opt='-O2',tests=True));continue
        submitted=source
        if re.search(r'^\s*#include\s+"',path.read_text(encoding='utf-8'),re.M):
            submitted='online_'+source.replace('/','_');flatten(task,source,submitted)
        tests='Tests tests(' in (ROOT/task/submitted).read_text(encoding='utf-8')
        records.append(verify(task,source,expected=EXPECTED.get(relative),tests=tests))
        if submitted!=source:records.append(verify(task,submitted,tests=tests))
        manifest.append({'base_source':relative,'submitted_source':task+'/'+submitted,'task_folder':task,
                         'local_stdout':task+'/outputs/'+submitted.removesuffix('.cpp').replace('/','_')+'_actual.txt',
                         'output_stem':source.removesuffix('.cpp').replace('/','_'),
                         'comparison':'Exact fixed stdout; sizeof/alignof/requested-payload numbers are explicit platform observations'})
    (ROOT/'Reference/programiz_manifest.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8',newline='\n')
    (ROOT/'Reference/benchmark_online_equivalents.json').write_text(json.dumps(BENCHMARK_EQUIVALENTS,indent=2)+'\n',encoding='utf-8',newline='\n')
    base=(ROOT/'PartB/TaskB1/outputs/main_actual.txt').read_bytes()
    extended=(ROOT/'PartB/TaskB4/outputs/main_actual.txt').read_bytes()
    assert extended.startswith(base),'Unchanged baseline output differs in extension run'
    (ROOT/'PartB/TaskB4/outputs/regression_summary.json').write_text(json.dumps({
        'status':'PASS','scope':'All earlier safe programs, exact predictions/TODOs, compiler diagnostics and optimized benchmarks rerun after extension; unsafe drivers remain sanitizer-only',
        'base_source_records':[r['source'] for r in records if not Path(r['source']).name.startswith('online_')],
        'normal_runtime_evaluations':sum(r['test_count'] for r in records if r.get('run_exit')==0),
        'baseline_stdout_prefix_matches':True},indent=2)+'\n',encoding='utf-8',newline='\n')
    print('Suitable Programiz programs:',len(manifest),'large timing drivers use documented small equivalents')

if __name__=='__main__':main()
