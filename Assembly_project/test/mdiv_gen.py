import sys
import random

def print_64_bits(x):
    print(f'{x:064b}')

def overflow_minint(test_id):
    n = test_id + 1
    if test_id >= 10:
        n += 1000

    print(n)
    for i in range(n - 1):
        print_64_bits(0)
    print_64_bits(1 << 63)
    print(-1)

def div_by_zero(test_id):
    if test_id <= 3:
        print(test_id + 1)
        for i in range(test_id + 1):
            print_64_bits(0)

    elif test_id <= 7:
        print(test_id - 3)
        for i in range(test_id - 3):
            print_64_bits((1 << 64) - 1)

    elif test_id <= 11:
        random.seed(test_id)
        print(test_id - 7)
        for i in range(test_id - 7):
            print_64_bits(random.randint(0, (1 << 64) - 1))
    else:
        random.seed(test_id)
        n = random.randint(100, 500)
        print(n)
        for i in range(n):
            print_64_bits(random.randint(0, (1 << 64) - 1))

    print(0)

def no_overflow_minint(test_id):
    n = test_id + 1
    if test_id >= 10:
        n += 1000

    print(n)
    for i in range(n - 1):
        print_64_bits(0)
    print_64_bits(1 << 63)
    print(1)

def totally_random(test_id):
    random.seed(test_id)
    n = random.randint(1, 100)
    print(n)
    for i in range(n):
        print_64_bits(random.randint(0, (1 << 64) - 1))
    print(random.randint(-(1 << 63), (1 << 63) - 1))

def zero_max_random(test_id):
    random.seed(test_id)
    n = random.randint(1, 3)
    if test_id >= 10:
        n = random.randint(1, 100)
    print(n)
    for i in range(n):
        t = random.randint(1, 5)
        if t == 1:
            print_64_bits(0)
        if t == 2:
            print_64_bits((1 << 64) - 1)
        
        if t > 2:
            print_64_bits(random.randint(0, (1 << 64) - 1))

    print(random.randint(-(1 << 63), (1 << 63) - 1))

def positive_edge_cases(test_id):
    random.seed(test_id)
    if test_id <= 3:
        print(test_id + 1)
        for i in range(test_id + 1):
            print_64_bits((1 << 64) - 1)
        print(random.randint(2, 3) * (-1 + 2 * random.randint(0, 1)))
    elif test_id <= 7:
        print(test_id - 3)
        for i in range(test_id - 3 - 1):
            print_64_bits(0)
        print_64_bits((1 << 64) - 1)
        print(random.randint(1, 5))
    elif test_id <= 11:
        print(test_id - 7)
        for i in range(test_id - 7):
            print_64_bits(0)
        print(random.randint(1, 3) * (-1 + 2 * random.randint(0, 1)))
    elif test_id <= 15:
        print(test_id - 11)
        for i in range(test_id - 11 - 1):
            print_64_bits(random.randint(0, (1 << 64) - 1))
        print_64_bits(0)
        print(random.randint(1, 3) * (-1 + 2 * random.randint(0, 1)))
    else:
        print(test_id - 15)
        for i in range(test_id - 15 - 1):
            print_64_bits(random.randint(0, (1 << 64) - 1))
        print_64_bits(1 << 63)
        print(random.randint(1, 3) * (-1 + 2 * random.randint(0, 1)))

tests = [overflow_minint, div_by_zero, no_overflow_minint, totally_random, zero_max_random, positive_edge_cases]


if len(sys.argv) <= 2:
    print("Error: Too few arguments passed! Please provide two arguments: test_group_id, test_id");
    exit(0)

test_group_id = int(sys.argv[1])
test_id = int(sys.argv[2])

tests[test_group_id](test_id)
