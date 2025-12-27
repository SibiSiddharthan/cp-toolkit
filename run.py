import os
import sys
import subprocess
import filecmp
import glob

# --------------------------------------------------
# Compile Solution
# --------------------------------------------------

if len(sys.argv) != 2:
    print("Usage: python check.py <problem_name>")
    exit(1)

name = sys.argv[1]
src = f"{name}.cpp"
exe = f"{name}.exe"

if not os.path.exists(src):
    print(f"Source file {src} not found")
    exit(1)


def needs_compile():
    if not os.path.exists(exe):
        return True
    return os.path.getmtime(src) > os.path.getmtime(exe)


def compile():
    cmd = ["clang++", "-std=c++23", "-fsanitize=address", "-g", src, "-o", exe]
    res = subprocess.run(cmd)

    if res.returncode != 0:
        exit(1)


if needs_compile():
    compile()

# --------------------------------------------------
# Detect test cases
# --------------------------------------------------

single_in = f"{name}.in"
single_out = f"{name}.out"

tests = []

if os.path.exists(single_in) and os.path.exists(single_out):
    tests.append((single_in, single_out))
else:
    if len(name) == 1:
        ins = sorted(glob.glob(f"{name}[0-9]*.in"))
    else:
        ins = sorted(glob.glob(f"{name}.[0-9]*.in"))

    for infile in ins:
        idx = infile[:-3]
        outfile = f"{idx}.out"
        if os.path.exists(outfile):
            tests.append((infile, outfile))

# --------------------------------------------------
# Run tests
# --------------------------------------------------

ok = True

for i, (inp, outp) in enumerate(tests, 1):
    tmp = "__tmp.out"

    with open(inp, "r") as fin, open(tmp, "w") as fout:
        subprocess.run([exe] if os.name == "nt" else ["./" + exe], stdin=fin, stdout=fout)

    if filecmp.cmp(tmp, outp, shallow=False):
        print(f"✅ Test {i}: Accepted")
    else:
        print(f"❌ Test {i}: Wrong Answer")
        print("---- Your Output ----")
        print(open(tmp).read())
        print("---- Expected ----")
        print(open(outp).read())
        ok = False
        break

os.remove(tmp)

if not ok:
    exit(1)

print("All tests passed!")
exit(0)
