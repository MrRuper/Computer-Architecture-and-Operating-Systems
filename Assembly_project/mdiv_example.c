#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE(x) (sizeof x / sizeof x[0])

// To jest deklaracja testowanej funkcji.
int64_t mdiv(int64_t *x, size_t n, int64_t y);

// To jest struktura przechowująca dane do testów i oczekiwane wyniki.
typedef struct {
  size_t  const  n; // rozmiar dzielnej
  int64_t const *x; // dzielna
  int64_t const  y; // dzielnik
  int64_t const *z; // oczekiwany iloraz
  int64_t const  r; // oczekiwana reszta
} test_data_t;

// Tu są dane do testów i oczekiwane wyniki.
static const test_data_t test_data[] = {
  {1, (int64_t[1]){ 13},  5, (int64_t[1]){ 2},  3},
  {1, (int64_t[1]){-13},  5, (int64_t[1]){-2}, -3},
  {1, (int64_t[1]){ 13}, -5, (int64_t[1]){-2},  3},
  {1, (int64_t[1]){-13}, -5, (int64_t[1]){ 2}, -3},
  {2, (int64_t[2]){0,  13},  5, (int64_t[2]){0x9999999999999999,  2},  3},
  {2, (int64_t[2]){0, -13},  5, (int64_t[2]){0x6666666666666667, -3}, -3},
  {2, (int64_t[2]){0,  13}, -5, (int64_t[2]){0x6666666666666667, -3},  3},
  {2, (int64_t[2]){0, -13}, -5, (int64_t[2]){0x9999999999999999,  2}, -3},
  {3, (int64_t[3]){1, 1, 1}, 2, (int64_t[3]){0x8000000000000000, 0x8000000000000000, 0},  1},
  {1, (int64_t[1]){65}, 0, (int64_t[1]){2}, 3},
  //{3, (int64_t[3]){0,0,0x8000000000000000}, -1, (int64_t[3]){0x8000000000000000, 0x8000000000000000, 0},  1}

};

int main() {
  for (size_t test = 0; test < SIZE(test_data); ++test) {
    size_t n = test_data[test].n;
    int64_t *work_space = malloc(n * sizeof (int64_t));
    assert(work_space);
    memcpy(work_space, test_data[test].x, n * sizeof (int64_t));

    int64_t r = mdiv(work_space, n, test_data[test].y);

    bool pass = true;
    if (r != test_data[test].r) {
      pass = false;
      printf("W teście %zu reszta\n"
             "jest        %" PRIi64 ",\n"
             "powinna być %" PRIi64 ".\n",
             test, r, test_data[test].r);
    }
    for (size_t i = 0; i < n; ++i) {
      if (work_space[i] != test_data[test].z[i]) {
        pass = false;
        printf("W teście %zu w ilorazie pod indeksem %zu\n"
               "jest        0x%016" PRIx64 ",\n"
               "powinno być 0x%016" PRIx64 ".\n",
               test, i, work_space[i], test_data[test].z[i]);
      }
    }
    free(work_space);

    if (pass) {
      printf("Test %zu zakończył się poprawnie.\n", test);
    }
  }
}
