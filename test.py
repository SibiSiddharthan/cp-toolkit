import random


def generate_random_ints(count: int, lower: int, upper: int, sum: int = None, exclude: set[int] = None):

    result = []
    current = 0
    base = lower
    range = upper - lower

    if sum != None:
        if base != 0:
            range = sum // (count * base)

    for _ in range(count):

        if sum != None:
            range -= current

        if exclude is None:
            value = random.randint(0, range)
            current += value
            result.append(base + value)
        else:
            while True:
                value = random.randint(0, range)

                if (base + value) not in exclude:
                    current += value
                    result.append(base + value)
                    break

    return result


def generate_random_floats(count: int, lower: float, upper: float):

    return [random.uniform(lower, upper) for _ in range(count)]


def generate_random_string(size: int, chars: list[str] = "abcdefghijklmnopqrstuvwxyz"):
    return ''.join(random.choice(chars) for _ in range(size))


def generate_random_grid(height, width, chars: list[str] = ".#"):

    grid = []

    for _ in range(height):
        grid.append(''.join(random.choice(chars) for _ in range(width)))

    return grid

