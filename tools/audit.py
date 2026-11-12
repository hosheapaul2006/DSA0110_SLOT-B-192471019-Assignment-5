"""Verify saved evidence without rerunning costly benchmarks or browser work."""
from pathlib import Path
import datetime,hashlib,json,re,subprocess,statistics,csv
from PIL import Image
from verify import ROOT,dependencies
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def read(p):return (ROOT/p).read_text(encoding='utf-8')
def data(p):return json.loads(read(p))
def git(*args):return subprocess.check_output(['git',*args],cwd=ROOT)
def normal(s):
    return '\n'.join(re.sub(r'\d+','<size>',l) if l.startswith('sizeof') else re.sub(r'(requested payload bytes=)\d+',r'\1<size>',l) for l in s.replace('\r\n','\n').split('\n'))
def main():
    records=[json.loads(p.read_text()) for p in ROOT.glob('Part*/Task*/outputs/*_verification.json') if not p.name.startswith('sanitizer_')]
    sources={p.relative_to(ROOT).as_posix() for d in ['PartA','PartB'] for p in (ROOT/d).rglob('*.cpp')}
    assert sources=={r['source'] for r in records}
    for r in records:
        assert sha(ROOT/r['source'])==r['sha256']
        assert dependencies(ROOT/r['source'])==r['headers_sha256']
        assert r['status'].startswith('PASS')
        assert all(f in r['compile_command'] for f in ['-std=c++17','-Wall','-Wextra','-Wpedantic'])
    runs=[r for r in records if r.get('run_exit')==0]
    assert sum(r['test_count'] for r in runs)==948
    assert sum(r['compile_exit']!=0 for r in records)==3
    assert sum(r['compile_only'] for r in records)==2
    baseline=data('Reference/baseline_before_B4.json')
    assert len(baseline['files'])==33
    for p,h in baseline['files'].items():
        assert sha(ROOT/p)==h==hashlib.sha256(git('show',baseline['baseline_source_commit']+':'+p)).hexdigest()
    assert read('PartB/TaskB4/outputs/main_actual.txt').startswith(read('PartB/TaskB1/outputs/main_actual.txt'))
    predictions=[]
    for task,label,commit in [('PartA/TaskA2','A2','7c78d17'),('PartA/TaskA5','A5','9000c57'),('PartB/TaskB2','B2','94ffa2c')]:
        assert read(task+'/predict_output.cpp')==read('Reference/'+label+'_prediction_original.txt')
        assert read(task+'/outputs/predict_output_predicted.txt')==read(task+'/outputs/predict_output_actual.txt')
        for p in [task+'/prediction_before_execution.md',task+'/outputs/predict_output_predicted.txt']:
            assert git('show',commit+':'+p).decode()==read(p)
        p=task+'/outputs/predict_output_verification.json'
        first=git('log','--reverse','--diff-filter=A','--format=%H','--',p).decode().splitlines()[0]
        observed=json.loads(git('show',first+':'+p))['timestamp_utc']
        committed=git('show','-s','--format=%cI',commit).decode().strip()
        assert datetime.datetime.fromisoformat(committed)<datetime.datetime.fromisoformat(observed)
        predictions.append({'task':task,'prediction_commit':commit,'first_execution_utc':observed})
    for label,count in [('B1',3),('B2',2)]:
        rebuilt=read('Reference/'+label+'_TODO_original.txt')
        for n in range(1,count+1):
            pattern=r'(?m)^[ \t]*// TODO '+str(n)+r':[^\n]*\n(?:[ \t]*//[^\n]*\n)*[ \t]*\n'
            rebuilt,m=re.subn(pattern,lambda m:read('Reference/'+label+'_completed_TODO'+str(n)+'.txt'),rebuilt)
            assert m==1
        assert rebuilt==read('PartB/Task'+label+'/complete_code.cpp'),label
    asan=data('Reference/sanitizer_summary.json')
    for r in asan:
        assert sha(ROOT/r['source'])==r['sha256'] and dependencies(ROOT/r['source'])==r['headers_sha256']
        assert r['compile_exit']==0 and '-fsanitize=address' in r['compile_command']
        p=Path(r['source']);stem='_'.join(p.parts[2:]).removesuffix('.cpp')
        stderr=read('/'.join(p.parts[:2])+'/outputs/sanitizer_'+stem+'_stderr.txt')
        if r['expectation']=='clean':assert r['run_exit']==0 and stderr=='' and r['status']=='PASS'
        elif r['expectation']=='expected_diagnostic':assert r['run_exit']!=0 and 'new-delete-type-mismatch' in stderr
        else:assert r['status'].startswith('OBSERVED: UB')
    assert [sum(r['expectation']==x for r in asan) for x in ['clean','expected_diagnostic','undefined_observation']]==[19,1,1]
    timed=0
    for p in ROOT.glob('PartB/TaskB*/outputs/*benchmark_O2_actual.txt'):
        rows=list(csv.DictReader(p.read_text().splitlines()));timed+=len(rows)
        for row in rows:
            assert float(row['elapsed_ms'])>=0
            if 'claims' in row:
                rejected=1000000*int(row['invalid_percent'])//100
                assert int(row['rejected'])==rejected and float(row['approved_sum'])==(1000000-rejected)*40000
            else:
                n,w=int(row['bills']),int(row['working_set'])
                assert w==min(n,65536)
                v=[(1700,50000,25000,11000,4000,7000,10000,3500,0,4500)[i%10]+(i//10)%97 for i in range(w)]
                assert float(row['checksum'])==(n//w)*sum(v)+sum(v[:n%w])
                assert int(row['requested_payload_bytes'])<=64*1024*1024
    assert timed==105
    for v in data('Reference/benchmark_summary.json').values():
        assert len(v['trials_ms'])==5 and statistics.median(v['trials_ms'])==v['median_ms']
        rows=list(csv.DictReader(read(v['source_csv']).splitlines()))
        group=[r for r in rows if r['method']==v['method'] and all(r[k]==x for k,x in v['workload'].items())]
        assert sorted(int(r['trial']) for r in group)==[1,2,3,4,5]
        assert [float(r['elapsed_ms']) for r in sorted(group,key=lambda r:int(r['trial']))]==v['trials_ms']
    inventory=data('Reference/screenshot_inventory.json')
    assert len(inventory)==24
    for s in inventory:
        assert sha(ROOT/s['path'])==s['sha256']
        with Image.open(ROOT/s['path']) as im:assert im.format=='JPEG' and im.size==(s['width'],s['height'])
        assert s['width']>=1920 and s['height']>=1080 and s['visual_review']
    manifest=data('Reference/programiz_manifest.json');markers=0
    for i in manifest:
        r=data(i['task_folder']+'/outputs/'+i['output_stem']+'_programiz.json')
        assert r['submitted_source']==i['submitted_source'] and sha(ROOT/i['submitted_source'])==r['submitted_sha256']
        assert sha(ROOT/i['base_source'])==r['base_sha256']
        actual=read(i['task_folder']+'/outputs/'+i['output_stem']+'_programiz.txt')
        assert hashlib.sha256(actual.encode()).hexdigest()==r['actual_output_sha256']
        a,e=normal(actual),normal(read(i['local_stdout']))
        if r['status']=='MATCH_WITH_ADDITIONAL_PROGRAMIZ_MARKER':
            assert re.search(r'^ERROR!\n',a,re.M);a=re.sub(r'^ERROR!\n','',a,flags=re.M);markers+=1
        else:assert r['status']=='PASS'
        assert a.startswith(e) and re.fullmatch(r'\s*(?:=== Code Execution Successful ===\s*)?',a[len(e):])
        for s in r['screenshots']:assert sha(ROOT/s['path'])==s['sha256']
    assert len(manifest)==19 and markers==2
    assert not any(p.name.lower()=='report' for p in ROOT.rglob('*') if p.is_dir())
    assert not any(p.suffix.lower() in ['.pdf','.docx'] for p in ROOT.rglob('*') if p.is_file() and p!=ROOT/'Reference/Assignment 5.pdf')
    assert not git('ls-files','*.exe','*.obj','*.o').strip()
    result={'status':'PASS','audited_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
      'cpp_sources':len(sources),'base_sources':27,'successful_compilations':sum(r['compile_exit']==0 for r in records),
      'safe_local_run_configurations':len(runs),'runtime_check_evaluations':948,'compiler_diagnostic_evaluations':5,
      'counting_note':'Canonical latest configurations, including repeated flattened-copy/B4 checks; historical retries excluded',
      'predictions':predictions,'protected_sources_unchanged':33,'actual_timed_loops':105,
      'sanitizer_cases':21,'sanitizer_clean':19,'sanitizer_expected_diagnostic':1,'sanitizer_undefined_observation':1,
      'programiz_distinct_functional_cases':19,'programiz_annotation_cases':2,
      'programiz_limitation':'B2 file cleanup is local/Ubuntu-only; 24-check online memory subset; relative-path failure retained; alternate paths unverified',
      'genuine_screenshots':24,'successful_or_annotated_views':23,'retained_failure_views':1,'native_diagrams':4,'no_report':True}
    (ROOT/'Reference/final_audit.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(result,indent=2))
if __name__=='__main__':main()
