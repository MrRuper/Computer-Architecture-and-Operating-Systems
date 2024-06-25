#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

int64_t mdiv(int64_t *x, size_t n, int64_t y);

typedef struct {
  size_t   n; // rozmiar dzielnej
  int64_t *x; // dzielna
  int64_t  y; // dzielnik
} test_data_t;

#define GROUPS 9
#define GROUP_SIZE 30

const size_t test_size[] = {
    10,             // 10
    100,            // 100
    500,            // 500
    10000,          // 10K
    50000,          // 50K
    100000,         // 100K
    500000,         // 500K
    1000000,        // 1M
    5000000,        // 5M
};

test_data_t tests[GROUPS * GROUP_SIZE];


uint64_t xorshift64() {
    static uint64_t x = 15651241621603518167u;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return x;
}

int64_t rand64_bits() {
    return xorshift64();
}

int is_wrong_test(test_data_t test) {
    if (test.y == 0)
        return 1;
    if (test.y != -1)
        return 0;
    if ((uint64_t)test.x[test.n - 1] != 0x8000000000000000)
        return 0;
    for (size_t i = 0; i < test.n - 1; i++)
        if (test.x[i] != 0)
            return 0;
    return 1;
}

test_data_t gen_test(int group_id) {
    test_data_t test;
    test.n = test_size[group_id];
    test.x = (int64_t*)malloc(sizeof(int64_t) * test.n);

    do {
        for(size_t i = 0; i < test.n; i++)
            test.x[i] = rand64_bits();
        test.y = rand64_bits();
    } while(is_wrong_test(test));

    return test;
}

void gen_tests() {
    printf(KMAG "Generating tests...\n" KNRM);
    for (size_t i = 0; i < GROUPS * GROUP_SIZE; i++) {
        if (i % GROUP_SIZE == 0)
            printf("\tGenerating group %ld\n", i / GROUP_SIZE);
        tests[i] = gen_test(i / GROUP_SIZE);
    }
    printf(KMAG "Tests have been generated\n" KNRM);
}

int main() {

    printf(KBLU "Measuring exeuction time of mdiv.asm\n" KNRM);
    printf("Note that this program does not validate the correctness of the mdiv function, but only measures its execution time.\n");

    gen_tests();

    for (size_t group = 0; group < GROUPS; group++) {
        printf("Running test group %ld:\n", group);
        double total_et = 0;
        for (size_t test_num = 0; test_num < GROUP_SIZE; test_num++) {
            test_data_t test = tests[group * GROUP_SIZE + test_num];

            clock_t start = clock();
            mdiv(test.x, test.n, test.y);
            clock_t end = clock();

            double et = ((double)(end - start)) * 1000 / CLOCKS_PER_SEC;

            total_et += et;
        }

        printf("\tAverage execution time: " KCYN "%lfms\n" KNRM, total_et / GROUP_SIZE);
    }

    return 0;
}
