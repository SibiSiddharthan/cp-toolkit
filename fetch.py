import os
import re
import subprocess
import sys

from bs4 import BeautifulSoup

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

CODEFORCES_REFERER = "https://codeforces.com"
ATCODER_REFERER = "https://atcoder.jp"

USER_AGENT = "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)"
ACCEPT_FORMATS = "Accept: text/html,application/xhtml+xml"

CODEFORCES_FETCH = False
ATCODER_FETCH = False

main_template = None

with open(f"{SCRIPT_DIR}/main.cpp", "rb") as file:
    main_template = file.read()


# Filename logic
def get_filename_codeforces(problem_index, test_index, num_tests):

    problem_index = problem_index.lower()

    if len(problem_index) == 1:
        if num_tests == 1:
            return problem_index
        else:
            return f"{problem_index}{test_index+1}"
    else:
        if num_tests == 1:
            return problem_index
        else:
            return f"{problem_index}.{test_index+1}"


def get_problem_letter_atcoder(h2_tag):
    # Example: "A - Problem Name" -> "a"
    if h2_tag:
        return h2_tag.text[0].lower()
    return None


if len(sys.argv) != 2:
    exit(1)

cmd = []
url = sys.argv[1]

if "codeforces" in url:
    CODEFORCES_FETCH = True
    cmd = ["curl", f"{url}/problems", "-H", USER_AGENT, "-H", ACCEPT_FORMATS, "-H", f"Referer: {CODEFORCES_REFERER}"]

if "atcoder" in url:
    ATCODER_FETCH = True
    cmd = ["curl", f"{url}/tasks_print", "-H", USER_AGENT, "-H", ACCEPT_FORMATS, "-H", f"Referer: {ATCODER_REFERER}"]

if len(cmd) == 0:
    exit(1)

result = subprocess.run(
    cmd,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
)

soup = BeautifulSoup(result.stdout, "html.parser")

if CODEFORCES_FETCH:

    problems = soup.find_all('div', class_='problemindexholder')

    for problem in problems:
        problem_index = problem.get('problemindex')
        if not problem_index:
            continue

        sample_tests = problem.find_all('div', class_='sample-test')

        for sample_test in sample_tests:
            input_blocks = sample_test.find_all('div', class_='input')
            output_blocks = sample_test.find_all('div', class_='output')

            num_tests = len(input_blocks)

            for i in range(0, num_tests):

                input_block = input_blocks[i]
                output_block = output_blocks[i]

                input_pre = input_block.find('pre') if input_block else None
                if input_pre:
                    input_lines = [line.get_text() for line in input_pre.find_all('div', class_='test-example-line')]
                    if not input_lines:
                        input_lines = [input_pre.get_text('\n')]
                    input_text = '\n'.join(input_lines).strip()
                    fname = f"{get_filename_codeforces(problem_index, i, num_tests)}.in"
                    with open(fname, 'w', encoding='utf-8') as f_in:
                        f_in.write(input_text)

                output_pre = output_block.find('pre') if output_block else None
                if output_pre:
                    output_lines = [line.get_text() for line in output_pre.find_all('div', class_='test-example-line')]
                    if not output_lines:
                        output_lines = [output_pre.get_text('\n')]
                    output_text = '\n'.join(output_lines).strip()
                    fname = f"{get_filename_codeforces(problem_index, i, num_tests)}.out"
                    with open(fname, 'w', encoding='utf-8') as f_out:
                        f_out.write(output_text)

        if not os.path.isfile(f"{problem_index.lower()}.cpp"):
            with open(f"{problem_index.lower()}.cpp", "wb") as file:
                file.write(main_template)

if ATCODER_FETCH:

    problems = soup.find_all("span", class_="h2")
    tasks = soup.find_all("div", id="task-statement")

    # Find all problem headers
    for i in range(0, len(problems)):
        prob_letter = get_problem_letter_atcoder(problems[i])
        if not prob_letter:
            continue

        inputs = []
        outputs = []

        # Find all sample input/output pairs after this h2
        # We'll look for <h3>Sample Input X</h3> and <h3>Sample Output X</h3>
        samples = tasks[i].find_all("div", class_="part")

        for sample in samples:
            sample_inputs = sample.find_all(lambda tag: tag.name == "h3" and re.match(r"^Sample Input( \d+)?$", tag.text))
            sample_outputs = sample.find_all(lambda tag: tag.name == "h3" and re.match(r"^Sample Output( \d+)?$", tag.text))

            if len(sample_inputs) != 0:
                input_pre = sample_inputs[0].find_next("pre")
                input_text = input_pre.get_text("\n").lstrip("\n").rstrip() if input_pre else ""
                inputs.append(input_text)

            if len(sample_outputs) != 0:
                output_pre = sample_outputs[0].find_next("pre")
                output_text = output_pre.get_text("\n").lstrip("\n").rstrip() if input_pre else ""
                outputs.append(output_text)

        for j in range(0, len(inputs)):
            fname = f"{prob_letter}{j+1}.in" if len(inputs) > 1 else f"{prob_letter}.in"
            with open(fname, "w", encoding="utf-8") as f_in:
                f_in.write(inputs[j])

        for j in range(0, len(outputs)):
            fname = f"{prob_letter}{j+1}.out" if len(outputs) > 1 else f"{prob_letter}.out"
            with open(fname, "w", encoding="utf-8") as f_in:
                f_in.write(outputs[j])

        if not os.path.isfile(f"{prob_letter}.cpp"):
            with open(f"{prob_letter}.cpp", "wb") as file:
                file.write(main_template)
