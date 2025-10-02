//
// Created by luizg on 02/10/2025.
//

#ifndef TESTS_H
#define TESTS_H

#define RUN_TEST(test_name) do { \
    printf("Runnig test: %s... ", #test_name); \
    if(test_name()) { \
        printf("PASSED\n"); \
    } else { \
        printf("FAILED\n"); \
        test_failed++; \
    } \
    test_run++; \
} while (0)



#endif //TESTS_H