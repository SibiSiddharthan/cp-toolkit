import os
import sys
import subprocess
import getopt
import resource
import time

compare = False
stress = False
optimized = False
iterations = 100

opts, args = getopt.getopt(sys.argv[1:], "cosn:")

for opt, val in opts:
    if opt == "-o":
        optimized = True
    elif opt == "-c":
        compare = True
    elif opt == "-s":
        stress = True
    elif opt == "-n":
        iterations = int(val)


def needs_compile(src, exe):
    if not os.path.exists(exe):
        return True
    return os.path.getmtime(src) > os.path.getmtime(exe)


def compile(src, exe):

    if not needs_compile():
        return

    if optimized:
        cmd = ["clang++", "-std=c++23", "-O3", "-rtlib=compiler-rt", "-DONLINE_JUDGE", "-DSELF_VALIDATE", src, "-o", exe]
    else:
        cmd = ["clang++", "-std=c++23", "-fsanitize=address", "-g", "-rtlib=compiler-rt", src, "-o", exe]
    res = subprocess.run(cmd)

    if res.returncode != 0:
        exit(1)


def check_file(file):
    if not os.path.exists(file):
        print(f"File {file} not found")
        exit(1)


if stress ^ compare:
    print("Usage: python verify.py [-c|-s] [-o] [-n <count>] <...>")
    print("Usage: python verify.py -c <generator> <source> <correct>")
    print("Usage: python verify.py -s <generator> <source>")

print(f"Running {iterations} Iterations")

if compare:
    if len(args) != 2:
        exit(1)

    generator = args[0]
    name = generator.replace(".py", "")

    source = args[1]
    src_exe = source.replace(".cpp", ".exe")

    correct = args[2]
    crt_exe = correct.replace(".cpp", ".exe")

    check_file(source)
    check_file(correct)
    check_file(generator)

    compile(source, src_exe)
    compile(correct, crt_exe)

    for _ in range(iterations):

        failed = False
        gen_result = subprocess.run(["python", generator], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        if gen_result.returncode != 0:
            print("Test Generation Failed")
            break

        testcase = gen_result.stdout.strip()
        src_result = subprocess.run(src_exe, input=testcase, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        crt_result = subprocess.run(crt_exe, input=testcase, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        if src_result.returncode != 0:
            print("Runtime Error")
            print(src_result.stdout)
            print(src_result.stderr)
            break

        if crt_result.returncode != 0:
            print("Runtime Error")
            print(crt_result.stdout)
            print(crt_result.stderr)
            break

        got = src_result.stdout.strip()
        exp = crt_result.stdout.strip()

        line_got = got.splitlines()
        line_exp = exp.splitlines()

        line_got = [line.strip() for line in line_got]
        line_exp = [line.strip() for line in line_exp]

        if len(line_got) != len(line_exp):
            failed = True

        for i in len(line_got):
            if line_got != line_exp:
                failed = True
                break

        if failed:
            print("Failed Testcase")

            with open(name + ".failed.in", "w") as file:
                file.write(testcase)

            with open(name + ".actual.out", "w") as file:
                file.write(got)

            with open(name + ".expected.out", "w") as file:
                file.write(exp)

            break

if stress:
    if len(args) != 3:
        exit(1)

    generator = args[0]
    name = generator.replace(".py", "")

    source = args[1]
    src_exe = source.replace(".cpp", ".exe")

    check_file(source)
    check_file(generator)

    for it in range(iterations):
        gen_result = subprocess.run(["python", generator], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        if gen_result.returncode != 0:
            print("Test Generation Failed")
            break

        testcase = gen_result.stdout.strip()

        start_time = time.time()
        #usage_start = resource.getrusage(resource.RUSAGE_CHILDREN)

        src_result = subprocess.run(src_exe, input=testcase, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        #usage_end = resource.getrusage(resource.RUSAGE_CHILDREN)
        end_time = time.time()

        time_taken = end_time - start_time
        #mem_usage_kb = usage_end.ru_maxrss - usage_start.ru_maxrss

        if src_result.returncode != 0:
            print("Runtime Error")

            with open(name + ".error.in", "w") as file:
                file.write(testcase)

            with open(name + ".error.out", "w") as file:
                file.write(src_result.stdout.strip())

            with open(name + ".error.err", "w") as file:
                file.write(src_result.stderr.strip())

            break

        print(f"Iteration: {it+1} Time: {time_taken:.4f}s, Peak Memory: {0} KB")
