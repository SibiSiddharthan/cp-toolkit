from bs4 import BeautifulSoup
import re

# Change this to your HTML file name if needed
HTML_FILE = "../o2"


def get_problem_letter(h2_tag):
    # Example: "A - Problem Name" -> "a"
    if h2_tag:
        return h2_tag.text[0].lower()
    return None


def main():
    with open(HTML_FILE, encoding="utf-8") as f:
        soup = BeautifulSoup(f, "html.parser")

    problems = soup.find_all("span", class_="h2")
    tasks = soup.find_all("div", id="task-statement")

    # Find all problem headers
    for i in range(0, len(problems)):
        prob_letter = get_problem_letter(problems[i])
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


if __name__ == "__main__":
    main()
