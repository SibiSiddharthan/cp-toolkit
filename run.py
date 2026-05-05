#   Copyright (c) 2024 - 2025 Sibi Siddharthan
#
#   Distributed under the MIT license.
#   Refer to the LICENSE file at the root directory for details.

import os
import sys
import subprocess
import glob
import getopt
import re

run_only = False
compile_only = False
optimized = False
multi_answer = False
fp_error = None
source = None
ins = []

opts, args = getopt.getopt(sys.argv[1:], "rcoms:t:f:")

for opt, val in opts:
    if opt == "-r":
        run_only = True
    elif opt == "-c":
        compile_only = True
    elif opt == "-o":
        optimized = True
    elif opt == "-m":
        multi_answer = True
    elif opt == "-s":
        source = val
        source = source.replace(".cpp", "")
    elif opt == "-t":
        if os.path.exists(val):
            ins.append(val)
        if os.path.exists(val + ".in"):
            ins.append(val + ".in")
    elif opt == "-f":
        fp_error = float(val)

# --------------------------------------------------
# Compile Solution
# --------------------------------------------------

if len(args) != 1:
    print("Usage: python run.py [-c] [-o] [-m] [-s source] [-t test] <problem_name>")
    exit(1)

name = args[0]

if source == None:
    source = name

src = f"{source}.cpp"
exe = f"{source}.exe"

if not os.path.exists(src):
    print(f"Source file {src} not found")
    exit(1)


def needs_compile():
    if not os.path.exists(exe):
        return True
    return os.path.getmtime(src) > os.path.getmtime(exe)


def compile():
    if optimized:
        cmd = ["clang++", "-std=c++23", "-O3", "-rtlib=compiler-rt", src, "-o", exe]
    else:
        cmd = ["clang++", "-std=c++23", "-fsanitize=address", "-g", "-rtlib=compiler-rt", src, "-o", exe]
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

if len(ins) == 0:

    if len(name) == 1:
        ins = sorted(glob.glob(f"{name}[0-9]*.in"))
    else:
        ins = sorted(glob.glob(f"{name}.[0-9]*.in"))

    if os.path.exists(f"{name}.in"):
        ins.append(f"{name}.in")

tests = []

for infile in ins:
    idx = infile[:-3]
    outfile = f"{idx}.out"

    if run_only:
        tests.append((infile, outfile))
        continue

    if os.path.exists(outfile):
        tests.append((infile, outfile))

# --------------------------------------------------
# Run tests
# --------------------------------------------------

ok = True

for i, (inp, outp) in enumerate(tests, 1):

    current = True
    incorrect = set()
    extra = set()
    missing = set()

    with open(inp, "r") as fin:
        if run_only:
            result = subprocess.run(exe, stdin=fin)
        else:
            result = subprocess.run(exe, stdin=fin, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    if result.returncode != 0:
        print(result.stderr)
        exit(1)

    if multi_answer:
        print(result.stdout)
        continue

    if run_only:
        continue

    got = result.stdout.strip()
    exp = open(outp).read().strip()

    line_got = got.splitlines()
    line_exp = exp.splitlines()

    line_got = [line.strip() for line in line_got]
    line_exp = [line.strip() for line in line_exp]

    got_idx = 0
    exp_idx = 0

    while got_idx < len(line_got) or exp_idx < len(line_exp):

        if got_idx == len(line_got):

            while exp_idx < len(line_exp):
                if line_exp[exp_idx].strip() == "":
                    exp_idx += 1
                    continue

                current = False
                missing.add(exp_idx)
                exp_idx += 1

            break

        if exp_idx == len(line_exp):

            while got_idx < len(line_got):
                if line_got[got_idx].strip() == "":
                    got_idx += 1
                    continue

                extra.add(got_idx)
                got_idx += 1

            break

        if line_got[got_idx].strip() == "":
            got_idx += 1
            continue

        if line_exp[exp_idx].strip() == "":
            exp_idx += 1
            continue

        if fp_error != None:

            def extract_floats(s):
                return [float(x) for x in re.findall(r'[-+]?\d*\.\d+(?:[eE][-+]?\d+)?|[-+]?\d+\.\d*|[-+]?\d+', s)]

            floats_got = line_got[got_idx].split()
            floats_exp = line_exp[exp_idx].split()

            floats_got = [float(x) for x in floats_got]
            floats_exp = [float(x) for x in floats_exp]

            if len(floats_got) != len(floats_exp):
                current = False
                incorrect.add(got_idx)

                continue

            for fg, fe in zip(floats_got, floats_exp):

                if abs(fg - fe) > fp_error:
                    current = False
                    incorrect.add(got_idx)

                    break

        else:
            if line_got[got_idx].strip().lower() != line_exp[exp_idx].strip().lower():
                current = False
                incorrect.add(got_idx)

        got_idx += 1
        exp_idx += 1

    print(f"Running Sample {inp}")
    print("---------------------------")
    print("Output")
    print("---------------------------")

    for i in range(0, len(line_got)):
        if i in incorrect:
            print(f"\033[91m{line_got[i]}\033[0m")
            continue

        if i in extra:
            print(f"\033[33m{line_got[i]}\033[0m")
            continue

        print(line_got[i])

    print("---------------------------")
    print("Expected")
    print("---------------------------")

    for i in range(0, len(line_exp)):
        if i in incorrect:
            print(f"\033[32m{line_exp[i]}\033[0m")
            continue

        if i in missing:
            print(f"\033[34m{line_exp[i]}\033[0m")
            continue

        print(line_exp[i])

    print("---------------------------")

    if current:
        print("✅ Accepted\n")
    else:
        print("❌ Wrong Answer\n")
        ok = False

if not ok:
    exit(1)

exit(0)
