"""Package this engine independently; no personal saves or UI binaries."""
import argparse
from pathlib import Path
import shutil
import subprocess
import zipfile

ROOT=Path(__file__).resolve().parents[1]

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build",type=Path,default=ROOT/"build-anybandui-native")
    parser.add_argument("--output",type=Path,default=ROOT/"build-anybandui-native/packages/angband-4.2.6")
    args=parser.parse_args()
    output=args.output.resolve()
    output.mkdir(parents=True,exist_ok=False)
    game=args.build.resolve()/"game"
    for name in ("angband-anybandui.exe","engine.anyband.json"):
        shutil.copy2(game/name,output/name)
    shutil.copytree(game/"lib",output/"lib",ignore=shutil.ignore_patterns("user","save","*.log"))
    shutil.copy2(ROOT/"docs/copying.rst",output/"copying.rst")
    shutil.copy2(ROOT/"anybandui/README.md",output/"README.md")
    candidates=sorted(Path("C:/Program Files/Microsoft Visual Studio").glob("*/*/VC/Redist/MSVC/[0-9]*/x64/Microsoft.VC*.CRT"))
    if candidates:
        for dll in candidates[-1].glob("*.dll"): shutil.copy2(dll,output/dll.name)
    # Exact corresponding working-tree source, including uncommitted integration.
    files=subprocess.check_output(["git","ls-files","--cached","--others","--exclude-standard","-z"],cwd=ROOT).decode().split("\0")
    with zipfile.ZipFile(output/"source.zip","w",zipfile.ZIP_DEFLATED) as archive:
        for name in sorted(set(files)):
            path=ROOT/name
            if name and path.is_file() and not path.is_relative_to(output): archive.write(path,"angband-4.2.6/"+name)
    dependency=ROOT/"build-anybandui/_deps/cjson-src"
    with zipfile.ZipFile(output/"cjson-source.zip","w",zipfile.ZIP_DEFLATED) as archive:
        for path in dependency.rglob("*"):
            if path.is_file() and ".git" not in path.parts: archive.write(path,"cjson/"+path.relative_to(dependency).as_posix())
    print(output)

if __name__=="__main__": main()
