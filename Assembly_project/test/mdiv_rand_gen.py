from random import randint

def gen_y():
    t = randint(0, 3)
    if t == 0:
        return -1 + 2 * randint(0, 1)
    if t == 1:
        return randint(1, 4) * (-1 + 2 * randint(0, 1))
    if t == 2:
        return randint(1, 50) * (-1 + 2 * randint(0, 1))
    return randint(1, (1 << 63) - 1) * (-1 + 2 * randint(0, 1))

n = randint(1, 5)
print(n)

t = randint(0, 2)
if t == 0:
    for i in range(n):
        x = randint(0, (1 << 64) - 1)
        print(f'{x:064b}')

if t == 1:
    for i in range(n):
        k = randint(1, 8)
        x = 0
        if k == 1:
            x = 0
        elif k == 2:
            x = (1 << 64) - 1
        else:
            x = randint(0, (1 << 64) - 1)
        print(f'{x:064b}')

if t == 2:
    for i in range(n):
        k = randint(1, 8)
        x = 0

        if k == 1:
            x = randint(-5, 5)
        if k == 2:
            x = (1 << 64) - 1
        else:
            x = randint(0, (1 << 64) - 1)
        print(f'{x:064b}')


print(gen_y())
