import os
from bs4 import BeautifulSoup


# Filename logic
def get_filename(problem_index, test_index, num_tests):

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


with open('../o1', encoding='utf-8') as f:
    soup = BeautifulSoup(f, 'html.parser')

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
                fname = f"{get_filename(problem_index, i, num_tests)}.in"
                with open(fname, 'w', encoding='utf-8') as f_in:
                    f_in.write(input_text)

            output_pre = output_block.find('pre') if output_block else None
            if output_pre:
                output_lines = [line.get_text() for line in output_pre.find_all('div', class_='test-example-line')]
                if not output_lines:
                    output_lines = [output_pre.get_text('\n')]
                output_text = '\n'.join(output_lines).strip()
                fname = f"{get_filename(problem_index, i, num_tests)}.out"
                with open(fname, 'w', encoding='utf-8') as f_out:
                    f_out.write(output_text)
