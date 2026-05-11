import os
import sys
import subprocess
import getopt
import time

compare = False
stress = False
optimized = False
fp_error = None
iterations = 100
time_limit = None
memory_limit = None

opts, args = getopt.getopt(sys.argv[1:], "cosn:f:t:m:")

for opt, val in opts:
    if opt == "-o":
        optimized = True
    elif opt == "-c":
        compare = True
    elif opt == "-s":
        stress = True
    elif opt == "-n":
        iterations = int(val)
    elif opt == "-f":
        fp_error = float(val)
    elif opt == "-t":
        time_limit = float(val)
    elif opt == "-m":
        memory_limit = int(val) * 1024


def get_peak_memory(proc):
    if os.name == "nt":
        # Windows
        import ctypes
        from ctypes import wintypes

        class PROCESS_MEMORY_COUNTERS(ctypes.Structure):
            _fields_ = [
                ("cb", wintypes.DWORD),
                ("PageFaultCount", wintypes.DWORD),
                ("PeakWorkingSetSize", ctypes.c_size_t),
                ("WorkingSetSize", ctypes.c_size_t),
                ("QuotaPeakPagedPoolUsage", ctypes.c_size_t),
                ("QuotaPagedPoolUsage", ctypes.c_size_t),
                ("QuotaPeakNonPagedPoolUsage", ctypes.c_size_t),
                ("QuotaNonPagedPoolUsage", ctypes.c_size_t),
                ("PagefileUsage", ctypes.c_size_t),
                ("PeakPagefileUsage", ctypes.c_size_t),
            ]

        GetProcessMemoryInfo = ctypes.windll.psapi.GetProcessMemoryInfo

        counters = PROCESS_MEMORY_COUNTERS()
        counters.cb = ctypes.sizeof(counters)
        GetProcessMemoryInfo(wintypes.HANDLE(proc._handle), ctypes.byref(counters), counters.cb)

        return counters.PeakWorkingSetSize

    else:
        # Linux / macOS
        import resource

        usage = resource.getrusage(resource.RUSAGE_CHILDREN)
        if sys.platform == "darwin":
            return usage.ru_maxrss
        else:
            return usage.ru_maxrss * 1024


def needs_compile(src, exe):
    if not os.path.exists(exe):
        return True
    return os.path.getmtime(src) > os.path.getmtime(exe)


def compile(src, exe, optimze):

    if not needs_compile(src, exe):
        return

    if optimze:
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


if stress ^ compare == 0:
    print("Usage: python verify.py [-c|-s] [-o] [-n <count>] <...>")
    print("Usage: python verify.py -c <generator> <source> <correct>")
    print("Usage: python verify.py -s <generator> <source>")
    exit(1)

print(f"Running {iterations} Iterations")

if compare:
    if len(args) != 3:
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

    compile(source, src_exe, optimized)
    compile(correct, crt_exe, True)

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

        for g, e in zip(line_got, line_exp):

            if fp_error != None:
                floats_got = g.split()
                floats_exp = e.split()

                floats_got = [float(x) for x in floats_got]
                floats_exp = [float(x) for x in floats_exp]

                if len(floats_got) != len(floats_exp):
                    failed = True
                    break

                for fg, fe in zip(floats_got, floats_exp):

                    if abs(fg - fe) > fp_error:
                        failed = True
                        break

                if failed:
                    break

            else:
                if g.strip().lower() != e.strip().lower():
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

            exit(1)

if stress:
    if len(args) != 2:
        exit(1)

    generator = args[0]
    name = generator.replace(".py", "")

    source = args[1]
    src_exe = source.replace(".cpp", ".exe")

    check_file(source)
    check_file(generator)

    compile(source, src_exe, optimized)

    for it in range(iterations):
        gen_result = subprocess.run(["python", generator], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        if gen_result.returncode != 0:
            print("Test Generation Failed")
            break

        testcase = gen_result.stdout.strip()

        start_time = start = time.perf_counter()

        process = subprocess.Popen(src_exe, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        out, err = process.communicate(testcase)
        process.wait()

        end_time = time.perf_counter()

        time_taken = end_time - start_time
        peak_memory = get_peak_memory(process) // 1024

        if process.returncode != 0:
            print("Runtime Error")

            with open(name + ".error.in", "w") as file:
                file.write(testcase)

            with open(name + ".error.out", "w") as file:
                file.write(out.strip())

            with open(name + ".error.err", "w") as file:
                file.write(err.strip())

            exit(1)

        if time_limit != None and memory_limit != None:
            if time_taken < time_limit and peak_memory < memory_limit:
                print(f"\033[32mIteration: {it+1}, Time: {time_taken:.4f}s, Peak Memory: {peak_memory:,} KB\033[0m")
            else:
                print(f"\033[91mIteration: {it+1}, Time: {time_taken:.4f}s, Peak Memory: {peak_memory:,} KB\033[0m")

                with open(f"{name}.stress.{it+1}.in", "w") as file:
                    file.write(testcase)
        else:
            print(f"Iteration: {it+1}, Time: {time_taken:.4f}s, Peak Memory: {peak_memory:,} KB")

exit(0)
