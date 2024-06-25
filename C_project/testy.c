#include "nand.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef ECANCELED
#define ECANCELED 105
#endif

#define TEST1
#define TEST2
#define TEST3
#define TEST4
#define TEST5
#define TEST6
#define TEST7
#define TEST8
#define TEST9
#define TEST10
#define TEST11
#define TEST12
#define TEST13
#define TEST14
#define TEST15
#define TEST16
#define TEST17
#define TEST18
#define TEST19


void testy() {
    // TESTY NA POPRAWNOŚĆ POŁĄCZEŃ, ZAKŁADAJĄC BRAK BŁĘDÓW
    printf("Uwaga! Najlepiej uruchamiac z valgrindem\n");

    // TEST 1
#ifdef TEST1
    {
        nand_t* g0 = nand_new(7);
        nand_t* g1 = nand_new(0);
        nand_t* g2 = nand_new(1);

        bool b = false;
        assert(g0 != NULL && g1 != NULL && g2 != NULL);

        errno = 1;
        bool uninitialed_input = nand_input(g0, 6) != NULL || nand_input(g2, 0) != NULL;

        // nand_input() ma ustawiać errno na zero w przypadku gdy zwraca pustą wartość
        bool errno_not_set_to_zero = errno != 0; 
        assert(!uninitialed_input && !errno_not_set_to_zero);

        // Krok 1
        nand_connect_signal(&b, g0, 0);
        nand_connect_nand(g1, g0, 2);
        nand_connect_nand(g1, g0, 4);
        nand_connect_nand(g2, g0, 3);
        nand_connect_nand(g0, g0, 5);
        nand_connect_nand(g0, g0, 6);

        assert(nand_input(g0, 0) == &b);
        assert(nand_input(g0, 1) == NULL);
        assert(nand_input(g0, 2) == g1);
        assert(nand_input(g0, 3) == g2);
        assert(nand_input(g0, 4) == g1);
        assert(nand_input(g0, 5) == g0);
        assert(nand_input(g0, 6) == g0);

        assert(nand_output(g1, 0) == g0);
        assert(nand_output(g1, 1) == g0);
        assert(nand_output(g2, 0) == g0);
        assert(nand_output(g0, 0) == g0);
        assert(nand_output(g0, 1) == g0);
        
        assert(nand_fan_out(g0) == 2);
        assert(nand_fan_out(g1) == 2);
        assert(nand_fan_out(g2) == 1);

        // Krok 2
        nand_delete(g0);
        nand_delete(g1);
        nand_delete(g2);

        printf("przeszedl TEST 1\n");
    }
#endif

    // TEST 2
#ifdef TEST2
    {
        nand_t* g0 = nand_new(7);
        nand_t* g1 = nand_new(0);
        nand_t* g2 = nand_new(1);

        bool b = false;

        // Krok 1
        nand_connect_signal(&b, g0, 0);
        nand_connect_nand(g1, g0, 2);
        nand_connect_nand(g1, g0, 4);
        nand_connect_nand(g2, g0, 3);
        nand_connect_nand(g0, g0, 5);
        nand_connect_nand(g0, g0, 6);

        // Krok 2
        nand_connect_signal(&b, g0, 0);
        nand_connect_signal(&b, g0, 1);
        nand_connect_signal(&b, g0, 2);
        nand_connect_signal(&b, g0, 3);
        nand_connect_signal(&b, g0, 4);
        nand_connect_signal(&b, g0, 5);
        nand_connect_signal(&b, g0, 6);

        assert(nand_input(g0, 0) == &b);
        assert(nand_input(g0, 1) == &b);
        assert(nand_input(g0, 2) == &b);
        assert(nand_input(g0, 3) == &b);
        assert(nand_input(g0, 4) == &b);
        assert(nand_input(g0, 5) == &b);
        assert(nand_input(g0, 6) == &b);

        assert(nand_fan_out(g0) == 0);
        assert(nand_fan_out(g1) == 0);
        assert(nand_fan_out(g2) == 0);

        // Krok 3
        nand_delete(g1);
        nand_delete(g2);
        nand_delete(g0);

        printf("przeszedl TEST 2\n");
    }
#endif

    // TEST 3
#ifdef TEST3
    {
        nand_t* g0 = nand_new(7);
        nand_t* g1 = nand_new(0);
        nand_t* g2 = nand_new(1);

        bool b = false;

        // Krok 1
        nand_connect_signal(&b, g0, 0);
        nand_connect_nand(g1, g0, 2);
        nand_connect_nand(g1, g0, 4);
        nand_connect_nand(g2, g0, 3);
        nand_connect_nand(g0, g0, 5);
        nand_connect_nand(g0, g0, 6);

        // Krok 2
        nand_connect_nand(g2, g0, 2);
        nand_connect_nand(g2, g0, 4);

        assert(nand_fan_out(g1) == 0);

        nand_connect_nand(g2, g0, 3);
        nand_connect_nand(g2, g0, 5);
        nand_connect_nand(g2, g0, 6);

        assert(nand_fan_out(g2) == 5);

        nand_connect_nand(g1, g0, 0);
        nand_connect_nand(g1, g0, 1);

        assert(nand_fan_out(g1) == 2);

        // Krok 3
        nand_delete(g1);
        nand_delete(g2);
        nand_delete(g0);

        printf("przeszedl TEST 3\n");
    }
#endif

    // TEST 4
#ifdef TEST4
    {
        nand_t** g = malloc(sizeof(nand_t*) * 100);

        g[0] = nand_new(99);

        for (int i = 1; i < 100; i++) {
            g[i] = nand_new(1);
            nand_connect_nand(g[i], g[0], i - 1);
            nand_connect_nand(g[0], g[i], 0);
        }

        assert(nand_fan_out(g[0]) == 99);

        for (int i = 0; i < 99; i++)
            assert(nand_input(g[0], i) == g[i + 1]);

        for (int i = 1; i < 100; i++)
            assert(nand_input(g[i], 0) == g[0]);

        for (int i = 0; i < 100; i++)
            nand_delete(g[i]);

        free(g);
        printf("przeszedl TEST 4\n");
    }
#endif

    // TESTY NA POPRAWNOŚĆ POŁĄCZEŃ, ZAKŁADAJĄC BŁĘDNE ARGUMENTY ////////////////////////////////////////////////////////////////////////////////////////////
    
    // TEST 5
#ifdef TEST5
    {
        nand_t* g0 = nand_new(0);
        nand_t* g1 = nand_new(2);
        nand_t* g2 = nand_new(2);

        bool b = false;

        nand_connect_nand(g1, g1, 1);
        nand_connect_nand(g1, g2, 0);
        nand_connect_nand(g1, g2, 1);

        errno = 0;
        assert(nand_connect_nand(NULL, g1, 0) == -1 && errno == EINVAL);
        errno = 0;
        assert(nand_connect_nand(g1, NULL, 0) == -1 && errno == EINVAL);
        errno = 0;
        assert(nand_connect_nand(g1, g2, 3) == -1 && errno == EINVAL);
        errno = 0;

        errno = 0;
        assert(nand_connect_signal(NULL, g1, 0) == -1 && errno == EINVAL);
        errno = 0;
        assert(nand_connect_signal(&b, NULL, 0) == -1 && errno == EINVAL);
        errno = 0;
        assert(nand_connect_signal(&b, g2, 3) == -1 && errno == EINVAL);
        errno = 0;
        assert(nand_fan_out(NULL) == -1 && errno == EINVAL);
        errno = 0;
        assert(nand_input(NULL, 0) == NULL && errno == EINVAL);
        errno = 0;
        assert(nand_input(g1, 2) == NULL && errno == EINVAL);
        errno = 0;

        // dla nand_output(), wynik ma być niezdefiniowany dla niepoprawnego inputu wedle specyfikacji
        //assert(nand_output(NULL, 0) == NULL && errno == EINVAL);
        //errno = 0;
        //assert(nand_output(g1, 2) == NULL && errno == EINVAL);
        //errno = 0;
        //assert(nand_output(g1, -1) == NULL && errno == EINVAL); // problem z porównaniem (ssize_t)(-1) < (size_t)(2) == 0 ?
        //errno = 0;

        nand_delete(g0);
        nand_delete(g1);
        nand_delete(g2);
        
        printf("przeszedl TEST 5\n");
    }
#endif
    // TEST 6
#ifdef TEST6
    {
        nand_t* g = nand_new(0);
        bool b;

        errno = 0;
        assert(nand_evaluate(NULL, &b, 1) == -1 && errno == EINVAL);
        errno = 0;
        assert(nand_evaluate(&g, NULL, 1) == -1 && errno == EINVAL);
        errno = 0;
        assert(nand_evaluate(NULL, NULL, 0) == -1 && errno == EINVAL);
        errno = 0;
		
		assert(nand_evaluate(&g, &b, 0) == -1 && errno == EINVAL);	
        errno = 0;

        nand_delete(g);

        printf("przeszedl TEST 6\n");
    }
#endif

    // TEST 7
#ifdef TEST7
    {
        nand_t** g = malloc(sizeof(nand_t*) * 2);
        g[0] = NULL;
        g[1] = NULL;
        bool b;
        
        errno = 0;
        assert(nand_evaluate(g, &b, 2) == -1 && errno != 0);
        errno = 0;

        free(g);

        printf("przeszedl TEST 7\n");
    }
#endif

    // TEST 8 
#ifdef TEST8
    {
        nand_t* g0 = nand_new(0);
        nand_t* g1 = nand_new(1);
        nand_t* g2 = nand_new(1);
        nand_t* g3 = nand_new(1);

        bool b0 = 0;
        bool b1 = 1;

        nand_connect_signal(&b0, g1, 0);
        nand_connect_signal(&b1, g2, 0);

        bool b = 1;

        errno = 0;
        assert(nand_evaluate(&g0, &b, 1) == 0);
        assert(b == 0);
        assert(nand_evaluate(&g1, &b, 1) == 1);
        assert(b == 1);
        assert(nand_evaluate(&g2, &b, 1) == 1);
        assert(b == 0);
        assert(nand_evaluate(&g3, &b, 1) == -1 && errno != 0);
        errno = 0;

        
        nand_delete(g0);
        nand_delete(g1);
        nand_delete(g2);
        nand_delete(g3);

        printf("przeszedl TEST 8\n");
    }
#endif

    // TEST 9
#ifdef TEST9
    {
        nand_t* g0 = nand_new(3);
        nand_t* g1 = nand_new(3);
        nand_t* g2 = nand_new(3);
        nand_t* g3 = nand_new(3);

        bool b0 = 0;
        bool b1 = 1;

        bool b = 0;

        nand_connect_signal(&b0, g0, 0);
        nand_connect_signal(&b0, g0, 1);
        nand_connect_signal(&b0, g0, 2);
        
        nand_connect_signal(&b1, g1, 0);
        nand_connect_signal(&b0, g1, 1);
        nand_connect_signal(&b0, g1, 2);

        nand_connect_signal(&b0, g2, 0);
        nand_connect_signal(&b1, g2, 1);
        nand_connect_signal(&b0, g2, 2);

        nand_connect_signal(&b1, g3, 0);
        nand_connect_signal(&b1, g3, 1);
        nand_connect_signal(&b1, g3, 2);

        assert(nand_evaluate(&g0, &b, 1) == 1);
        assert(b == 1);
        assert(nand_evaluate(&g1, &b, 1) == 1);
        assert(b == 1);
        assert(nand_evaluate(&g2, &b, 1) == 1);
        assert(b == 1);
        assert(nand_evaluate(&g3, &b, 1) == 1);
        assert(b == 0);

        nand_delete(g0);
        nand_delete(g1);
        nand_delete(g2);
        nand_delete(g3);

        printf("przeszedl TEST 9\n");
    }
#endif

    // TEST 10
#ifdef TEST10
    {
        nand_t* g0 = nand_new(1);
        nand_t* g1 = nand_new(2);

        bool b0 = 0;

        bool b;

        nand_connect_nand(g0, g0, 0);

        nand_connect_signal(&b0, g1, 0);
        nand_connect_nand(g1, g1, 1);

        errno = 0;
        assert(nand_evaluate(&g0, &b, 1) == -1 && errno == ECANCELED);
        errno = 0;
        assert(nand_evaluate(&g1, &b, 1) == -1 && errno == ECANCELED);
        errno = 0;

        nand_delete(g0);
        nand_delete(g1);

        printf("przeszedl TEST 10\n");
    }
#endif

    // TEST 11
#ifdef TEST11
    {
        nand_t* g0 = nand_new(2);
        nand_t* g1 = nand_new(1);

        bool b0 = 0;
        bool b;

        nand_connect_nand(g0, g1, 0);

        nand_connect_signal(&b0, g0, 0);
        nand_connect_nand(g1, g0, 1);

        errno = 0;
        assert(nand_evaluate(&g0, &b, 1) == -1 && errno == ECANCELED);
        errno = 0;
        
        nand_delete(g0);
        nand_delete(g1);

        printf("przeszedl TEST 11\n");
    }
#endif

    // TEST 12
#ifdef TEST12
    {
        nand_t* g0 = nand_new(1);
        nand_t* g1 = nand_new(2);

        bool b0 = 0;
        bool b;

        nand_connect_nand(g1, g0, 0);
        nand_connect_signal(&b0, g1, 0);

        errno = 0;
        assert(nand_evaluate(&g0, &b, 1) == -1 && errno == ECANCELED);
        errno = 0;

        nand_delete(g0);
        nand_delete(g1);

        printf("przeszedl TEST 12\n");
    }
#endif
    
    // TEST 13
#ifdef TEST13
    {
        nand_t* g0 = nand_new(2);
        nand_t* g1 = nand_new(1);
        nand_t* g2 = nand_new(1);
        nand_t* g3 = nand_new(1);

        bool b;

        nand_connect_nand(g1, g0, 0);
        nand_connect_nand(g2, g0, 1);
        
        nand_connect_nand(g3, g1, 0);
        
        nand_connect_nand(g3, g2, 0);
        
        nand_connect_nand(g2, g3, 0);

        errno = 0;
        assert(nand_evaluate(&g0, &b, 1) == -1 && errno == ECANCELED);
        errno = 0;

        nand_delete(g0);
        nand_delete(g1);
        nand_delete(g2);
        nand_delete(g3);

        printf("przeszedl TEST 13\n");
    }
#endif
    
    // TEST 14
#ifdef TEST14
    {
        nand_t* g0 = nand_new(2);
        nand_t* g1 = nand_new(1);
        nand_t* g2 = nand_new(1);
        nand_t* g3 = nand_new(1);

        bool b1 = 1;
        bool b = 1;

        nand_connect_nand(g1, g0, 0);
        nand_connect_nand(g2, g0, 1);
        
        nand_connect_nand(g3, g1, 0);
        
        nand_connect_nand(g3, g2, 0);
        
        nand_connect_signal(&b1, g3, 0);

        assert(nand_evaluate(&g0, &b, 1) == 3);
        assert(b == 0);

        nand_delete(g0);
        nand_delete(g1);
        nand_delete(g2);
        nand_delete(g3);

        printf("przeszedl TEST 14\n");
    }
#endif

    // TEST 15
#ifdef TEST15
    {
        nand_t* g0 = nand_new(4);
        nand_t* g1 = nand_new(1);

        bool b0 = 0;
        bool b1 = 1;
        bool b = 1;

        nand_connect_signal(&b0, g1, 0);
        nand_connect_signal(&b1, g0, 0);
        nand_connect_nand(g1, g0, 1);
        nand_connect_nand(g1, g0, 2);
        nand_connect_nand(g1, g0, 3);

        assert(nand_evaluate(&g0, &b, 1) == 2);
        assert(b == 0);

        nand_delete(g0);
        nand_delete(g1);
        
        printf("przeszedl TEST 15\n");
    }
#endif

    // TEST 16
#ifdef TEST16
    {
        nand_t* g0 = nand_new(3);
        nand_t* g1 = nand_new(1);
        nand_t* g2 = nand_new(1);
        nand_t* g3 = nand_new(1);

        bool b0 = 0;
        bool b = 1;

        nand_connect_nand(g1, g0, 0);
        nand_connect_nand(g2, g0, 1);
        nand_connect_nand(g3, g0, 2);

        nand_connect_signal(&b0, g1, 0);
        nand_connect_signal(&b0, g2, 0);
        nand_connect_signal(&b0, g3, 0);

        assert(nand_evaluate(&g0, &b, 1) == 2);
        assert(b == 0);

        nand_delete(g0);
        nand_delete(g1);
        nand_delete(g2);
        nand_delete(g3);

        printf("przeszedl TEST 16\n");
    }
#endif

    // TEST 17
#ifdef TEST17
    {
        nand_t* g0 = nand_new(2);
        nand_t* g1 = nand_new(1);
        nand_t* g2 = nand_new(1);
        nand_t* g3 = nand_new(1);
        nand_t* g4 = nand_new(1);
        nand_t* g5 = nand_new(1);
        nand_t* g6 = nand_new(1);
        nand_t* g7 = nand_new(0);
        nand_t* g8 = nand_new(1);
        nand_t* g9 = nand_new(1);
        nand_t* g10 = nand_new(1);
        nand_t* g11 = nand_new(1);

        bool b0 = 0;
        bool b;

        nand_connect_nand(g1, g0, 1);
        nand_connect_nand(g2, g1, 0);
        nand_connect_signal(&b0, g2, 0);
        
        nand_connect_nand(g4, g3, 1);
        nand_connect_nand(g5, g4, 0);
        nand_connect_nand(g6, g5, 0);
        nand_connect_nand(g7, g6, 0);

        nand_connect_nand(g3, g0, 0);
        nand_connect_nand(g8, g3, 0);
        nand_connect_nand(g9, g8, 0);
        nand_connect_nand(g10, g9, 0);
        nand_connect_nand(g11, g10, 0);
        nand_connect_nand(g5, g11, 0);

        assert(nand_evaluate(&g0, &b, 1) == 8);
        assert(b == 1);

        nand_delete(g0);
        nand_delete(g1);
        nand_delete(g2);
        nand_delete(g3);
        nand_delete(g4);
        nand_delete(g5);
        nand_delete(g6);
        nand_delete(g7);
        nand_delete(g8);
        nand_delete(g9);
        nand_delete(g10);
        nand_delete(g11);

        printf("przeszedl TEST 17\n");
    }
#endif

    // TEST 18
#ifdef TEST18
    {
        nand_t* g0 = nand_new(2);
        nand_t* g1 = nand_new(1);
        nand_t* g2 = nand_new(1);
        nand_t* g3 = nand_new(2);
        nand_t* g4 = nand_new(1);
        nand_t* g5 = nand_new(1);
        nand_t* g6 = nand_new(1);
        nand_t* g7 = nand_new(0);
        nand_t* g8 = nand_new(1);
        nand_t* g9 = nand_new(1);
        nand_t* g10 = nand_new(1);
        nand_t* g11 = nand_new(1);

        bool b0 = 0;

        nand_connect_nand(g1, g0, 1);
        nand_connect_nand(g2, g1, 0);
        nand_connect_signal(&b0, g2, 0);
        
        nand_connect_nand(g4, g3, 1);
        nand_connect_nand(g5, g4, 0);
        nand_connect_nand(g6, g5, 0);
        nand_connect_nand(g7, g6, 0);

        nand_connect_nand(g3, g0, 0);
        nand_connect_nand(g8, g3, 0);
        nand_connect_nand(g9, g8, 0);
        nand_connect_nand(g10, g9, 0);
        nand_connect_nand(g11, g10, 0);
        nand_connect_nand(g5, g11, 0);

        nand_t** g = malloc(sizeof(nand_t*) * 3);

        g[0] = g0;
        g[1] = g4;
        g[2] = g8;

        bool b[3] = {false, false, true};

        assert(nand_evaluate(g, b, 3) == 8);
        assert(b[0] == 1);
        assert(b[1] == 1);
        assert(b[2] == 0);

        free(g);

        nand_delete(g0);
        nand_delete(g1);
        nand_delete(g2);
        nand_delete(g3);
        nand_delete(g4);
        nand_delete(g5);
        nand_delete(g6);
        nand_delete(g7);
        nand_delete(g8);
        nand_delete(g9);
        nand_delete(g10);
        nand_delete(g11);

        printf("przeszedl TEST 18\n");
    }
#endif

#ifdef TEST19
    {
        bool s_in[3];
        bool s_out[1];
        nand_t* g1;
        nand_t* g2;
        nand_t* g3;

        g1 = nand_new(2);
        g2 = nand_new(3);

        nand_connect_signal(s_in + 0, g1, 0);
        nand_connect_signal(s_in + 1, g1, 1);

        s_in[0] = true;
        s_in[1] = false;
        s_in[2] = true;
        nand_t* test1[2];
        test1[0] = g1;
        test1[1] = g2;

        assert(nand_evaluate(&g1, s_out, 1) == 1 && s_out[0] == true);

        nand_connect_nand(g1, g2, 0);

        assert(nand_evaluate(&g1, s_out, 1) == 1 && s_out[0] == true);
        assert(nand_evaluate(&g2, s_out, 1) == -1);
        nand_connect_signal(s_in + 2, g2, 2);
        assert(nand_evaluate(&g2, s_out, 1) == -1);
        nand_t* all_nands[2];
        all_nands[0] = g1;
        all_nands[1] = g2;
        bool s[2];
        assert(nand_evaluate(all_nands,s, 2 ) == -1);
        nand_connect_nand(g1, g2, 1);
        assert(nand_evaluate(all_nands, s, 2) == 2);
        assert(nand_evaluate(&g1, s_out, 1) == 1);
        assert(nand_evaluate(&g2, s_out, 1) == 2);

        g3 = nand_new(2);

        nand_connect_nand(g2, g3, 0);
        nand_connect_signal(s_in + 2, g3, 1);

        assert(nand_evaluate(&g3, s_out, 1) == 3);
        nand_connect_nand(g1, g3, 0);
        assert(nand_evaluate(&g3, s_out, 1) == 2);
        assert(nand_fan_out(g2) == 0);
        nand_connect_nand(g3, g2, 1);

        struct nand* all[3];
        bool ss_out[3];
        all[0] = g1;
        all[1] = g2;
        all[2] = g3;

        assert(nand_evaluate(all, ss_out, 3) == 3 && ss_out[0] == true && ss_out[1] == true && ss_out[2] == false);
        nand_t* g4 = nand_new(2);
        nand_t* g5 = nand_new(2);
        nand_connect_nand(g2, g4, 0);
        nand_connect_nand(g2, g4, 1);
        nand_connect_nand(g4, g5, 0);
        nand_connect_nand(g4, g5, 1);

        nand_t* test2[] = {g1,g2,g3,g4,g5};
        bool test_out[5];

        assert(nand_evaluate(test2, test_out, 5) == 5);

        nand_connect_nand(g5, g4, 1);
        assert(nand_evaluate(test2, test_out, 5) == -1);
        nand_delete(g1);
        nand_delete(g2);
        nand_delete(g5);
        nand_delete(g4);
        nand_delete(g3);

        printf("przeszedl TEST 19 \n");
    }
    #endif

    printf("Zakonczono testowanie.");
}

int main(void) {
    testy();

    return 0;
}
