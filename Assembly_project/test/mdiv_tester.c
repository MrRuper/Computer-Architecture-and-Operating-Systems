#include <stdio.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdlib.h>

int64_t mdiv(int64_t *x, size_t n, int64_t y);

void print_64_bits(int64_t x) {
    for(int i = 0; i < 64; i++) {
        if((x >> (63 - i)) & 1) {
            putchar('1');
        } else {
            putchar('0');
        }
    }
    putchar('\n');
}

int main() {
    size_t n;
    if (!scanf("%ld", &n)) {
        printf("Input error\n");
        return 1;
    }

    int64_t *x = (int64_t*)malloc(sizeof(int64_t) * n);
    for(size_t i = 0; i < n; i++) {
        char c[65];
        
        if (!scanf("%s", c)) {
            printf("Input error\n");
            return 0;
        }
        x[i] = 0;
        for(int j = 0; j < 64; j++) {
            if(c[j] == '1') {
                x[i] |= ((int64_t)1) << (63 - j);
            }
        }
    }

    int64_t y;
    if (!scanf("%ld", &y)) {
        printf("Input errror\n");
        return 1;
    }

    int64_t rem = mdiv(x, n, y);

    printf("%ld\n", n);
    for(size_t i = 0; i < n; i++)
        print_64_bits(x[i]);
    printf("%ld\n", rem);
}
