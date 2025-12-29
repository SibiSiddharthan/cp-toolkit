import os
import sys
import subprocess
import glob
import getopt

compile_only = False
optimized = False
multi_answer = False

opts, args = getopt.getopt(sys.argv[1:], "com")

for opt, _ in opts:
    if opt == "-c":
        compile_only = True
    elif opt == "-o":
        optimized = True
    elif opt == "-m":
        multi_answer = True

# --------------------------------------------------
# Compile Solution
# --------------------------------------------------

if len(args) != 1:
    print("Usage: python check.py [-c] [-o] [-m] <problem_name>")
    exit(1)

name = args[0]
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
    if optimized:
        cmd = ["clang++", "-std=c++23", "-O3", src, "-o", exe]
    else:
        cmd = ["clang++", "-std=c++23", "-fsanitize=address", "-g", src, "-o", exe]
    res = subprocess.run(cmd)

    if res.returncode != 0:
        exit(1)


if needs_compile():
    compile()

if compile_only:
    exit(0)

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
    with open(inp, "r") as fin:
        result = subprocess.run(exe, stdin=fin, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    if result.returncode != 0:
        print(result.stderr)
        exit(1)

    if multi_answer:
        print(result.stdout)
        continue

    expected = open(outp).read()

    got = result.stdout.strip().splitlines()
    exp = expected.strip().splitlines()

print("All tests passed!")
exit(0)
