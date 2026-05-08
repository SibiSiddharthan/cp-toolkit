import random


def generate_random_ints(count: int, lower: int, upper: int, sum: int = None, exclude: set[int] = None):

    result = []
    current = 0

    lower = int(lower)
    upper = int(upper)

    base = lower
    max = upper - lower

    if sum != None:
        if base != 0:
            max = sum // (count * base)

    for _ in range(count):

        if sum != None:
            max -= current

        if exclude is None:
            value = random.randint(0, max)
            current += value
            result.append(base + value)
        else:
            while True:
                value = random.randint(0, max)

                if (base + value) not in exclude:
                    current += value
                    result.append(base + value)
                    break

    return result


def generate_random_floats(count: int, lower: float, upper: float):

    return [random.uniform(lower, upper) for _ in range(int(count))]


def generate_random_range(lower: int, upper: int, strict: bool = False):

    left = 0
    right = 0

    lower = int(lower)
    upper = int(upper)

    if strict:
        left = random.randint(lower, upper - 1)
        right = random.randint(left + 1, upper)
    else:
        left = random.randint(lower, upper)
        right = random.randint(left, upper)

    return (left, right)


def generate_random_string(size: int, chars: list[str] = "abcdefghijklmnopqrstuvwxyz"):
    return ''.join(random.choice(chars) for _ in range(int(size)))


def generate_random_grid(height: int, width: int, chars: list[str] = ".#"):

    grid = []

    for _ in range(int(height)):
        grid.append(''.join(random.choice(chars) for _ in range(int(width))))

    return grid


TESTS = 1
print(TESTS)


def generate_testcase():
    n = int(0)
    q = int(0)


for _ in range(TESTS):
    generate_testcase()
