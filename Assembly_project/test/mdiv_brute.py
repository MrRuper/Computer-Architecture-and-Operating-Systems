n = int(input())

x = 0
for i in range(n - 1):
    z = int(input(), 2)
    x |= z << (i * 64)

z = int(input(), 2)

if ((z >> 63) & 1) == 1:
    z ^= 1 << 63
    x |= z << ((n - 1) * 64)
    x -= 1 << ((n - 1) * 64 + 63)
else:
    x |= z << ((n - 1) * 64)

sign_x = True

if x < 0:
    sign_x = False
    x = -x

y = int(input())

sign_y = True

if y < 0:
    y = -y
    sign_y = False

rem = x % y
div = x // y

#print("div is equal to ", div)

if sign_x == False and sign_y == True:
    rem = -rem
    div = -div

if sign_x == True and sign_y == False:
    div = -div

if sign_x == False and sign_y == False:
    rem = -rem

print(n)
for i in range(n):
    z = (div >> (i * 64)) & ((1 << 64) - 1)
    print(f'{z:064b}')
print(rem)
