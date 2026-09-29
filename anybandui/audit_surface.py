"""Measure modifications to existing Angband files, separately from adapter code.
Usage: python -B anybandui/audit_surface.py [--base STOCK_REF] [--revision REF]
Without --revision, includes the current working tree. No files are modified.
"""
import argparse
import json
from pathlib import Path
import subprocess

ROOT=Path(__file__).resolve().parents[1]
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--base",default="f3082213b")
    parser.add_argument("--revision")
    args=parser.parse_args()
    command=["git","diff","--numstat",args.base]
    if args.revision: command.append(args.revision)
    command += ["--","src","CMakeLists.txt"]
    rows=subprocess.check_output(command,cwd=ROOT,text=True).splitlines()
    files=[]
    for row in rows:
        added,removed,name=row.split("\t",2)
        files.append({"path":name,"added":int(added),"removed":int(removed)})
    print(json.dumps({"stock":args.base,"revision":args.revision or "working-tree",
        "existing_files_changed":len(files),"added":sum(f["added"] for f in files),
        "removed":sum(f["removed"] for f in files),"files":files},indent=2))
if __name__=="__main__": main()
