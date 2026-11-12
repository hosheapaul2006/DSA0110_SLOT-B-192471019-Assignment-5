"""A disclosed memory-only adaptation for Programiz's unwritable filesystem."""
import json
from verify import ROOT, verify

def adapt(manifest):
    item = next(x for x in manifest if x['base_source'] == 'PartB/TaskB2/main.cpp')
    original = (ROOT/'PartB/TaskB2/online_main.cpp').read_text(encoding='utf-8')
    start = original.index('    const char* filename="b2_raii_resource.tmp";')
    end = original.index('    tests.check(ScopeTrace::live==0 && AllocationProbe::live==0',start)
    replacement = '''    // Programiz denied relative-path file creation. Full file RAII test
    // remains unchanged in main.cpp and passed locally and Ubuntu ASan.
    bool memoryExceptionCaught=false;
    try {
        ScopeTrace scope("memory-scope");
        auto resource=std::make_unique<AllocationProbe>();
        throw ClaimError("memory-scope failure");
    } catch(const ClaimError&) { memoryExceptionCaught=true; }
    tests.check(memoryExceptionCaught,"memory-only online unwind observed");
'''
    destination = ROOT/'PartB/TaskB2/online_main_programiz.cpp'
    adapted=original[:start]+replacement+original[end:]
    adapted=adapted.replace('RAII memory and file cleanup verified','RAII memory cleanup verified; file cleanup is local/Ubuntu-only')
    destination.write_text(adapted, encoding='utf-8', newline='\n')
    record = verify('PartB/TaskB2', destination.name, tests=True)
    item['submitted_source'] = 'PartB/TaskB2/'+destination.name
    item['local_stdout'] = 'PartB/TaskB2/outputs/online_main_programiz_actual.txt'
    item['adaptation'] = 'Memory-only online unwind subset: relative-path file creation failed. Alternate paths are unverified. Full file RAII remains verified by unchanged main.cpp locally and Ubuntu ASan. Original online failure retained.'
    return record

if __name__ == '__main__':
    path=ROOT/'Reference/programiz_manifest.json'
    manifest=json.loads(path.read_text(encoding='utf-8'))
    adapt(manifest)
    path.write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8',newline='\n')
