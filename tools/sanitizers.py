"""Isolated Linux sanitizer runs; export the actual evidence bytes losslessly."""
import base64
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import subprocess
from verify import dependencies

ROOT=Path(__file__).resolve().parents[1]
CASES=[
    ('PartA/TaskA2/predict_output.cpp','clean',None),
    ('PartA/TaskA2/layout_probe.cpp','clean',None),
    ('PartA/TaskA2/dynamic_cast_alternative.cpp','clean',None),
    ('PartA/TaskA2/experiments/nonvirtual_destructor.cpp','undefined_observation',None),
]

def main():
    files={};records=[];failed=False
    version=subprocess.run(['g++','--version'],capture_output=True,check=True).stdout
    files['Reference/sanitizer_environment.txt']=version+('\n'+platform.platform()+'\nCommit: '+os.environ.get('GITHUB_SHA','')+'\n').encode()
    build=ROOT/'build/sanitizer';build.mkdir(parents=True,exist_ok=True)
    for source,expectation,pattern in CASES:
        path=ROOT/source;parts=Path(source).parts
        prefix='/'.join(parts[:2])+'/outputs/sanitizer_'+'_'.join(parts[2:]).removesuffix('.cpp')
        exe=build/source.removesuffix('.cpp').replace('/','_')
        command=['g++','-std=c++17','-Wall','-Wextra','-Wpedantic','-g','-O1','-fsanitize=address','-fno-omit-frame-pointer',str(path),'-o',str(exe)]
        compiled=subprocess.run(command,capture_output=True,timeout=180)
        files[prefix+'_compiler.txt']=compiled.stdout+compiled.stderr
        record={'source':source,'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'headers_sha256':dependencies(path),
                'timestamp_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'compile_command':command,
                'compile_exit':compiled.returncode,'expectation':expectation,'expected_error_pattern':pattern,
                'runner':'GitHub Actions Ubuntu 24.04','commit':os.environ.get('GITHUB_SHA'),
                'run_url':'https://github.com/'+os.environ['GITHUB_REPOSITORY']+'/actions/runs/'+os.environ['GITHUB_RUN_ID']}
        if compiled.returncode!=0:
            record['status']='FAIL: compilation';failed=True
        else:
            env=os.environ.copy();env['ASAN_OPTIONS']='detect_leaks=1:halt_on_error=1:alloc_dealloc_mismatch=1:new_delete_type_mismatch=1'
            result=subprocess.run([str(exe)],capture_output=True,env=env,timeout=180)
            files[prefix+'_stdout.txt']=result.stdout;files[prefix+'_stderr.txt']=result.stderr
            stderr=result.stderr.decode(errors='replace')
            if expectation=='undefined_observation':
                # A nonvirtual delete is UB even if ASan emits nothing. Never label it clean.
                passed=True
                status=('OBSERVED: UB, no sanitizer diagnostic' if result.returncode==0 and not stderr
                        else 'OBSERVED: UB, nonzero exit or diagnostic recorded')
            else:
                passed=(result.returncode!=0 and re.search(pattern,stderr) is not None) if pattern else (result.returncode==0 and not stderr)
                status='PASS' if passed else 'FAIL'
            record.update(run_command=[str(exe)],run_exit=result.returncode,ASAN_OPTIONS=env['ASAN_OPTIONS'],status=status)
            failed=failed or not passed
        files[prefix+'_verification.json']=(json.dumps(record,indent=2)+'\n').encode()
        records.append(record);print(source,record['status'],flush=True)
    files['Reference/sanitizer_summary.json']=(json.dumps(records,indent=2)+'\n').encode()
    print('BEGIN_EVIDENCE_BUNDLE',flush=True)
    bundle={name:base64.b64encode(data).decode('ascii') for name,data in files.items()}
    print(base64.b64encode(json.dumps(bundle,separators=(',',':')).encode()).decode('ascii'),flush=True)
    print('END_EVIDENCE_BUNDLE',flush=True)
    if failed:raise SystemExit(1)

if __name__=='__main__':main()
