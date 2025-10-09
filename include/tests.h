//
// Created by luizg on 02/10/2025.
//

#ifndef TESTS_H
#define TESTS_H

int test_run = 0;
int test_failed = 0;

#define RUN_TEST(test_name) do { \
    printf("Runnig test: %s\n", #test_name); \
    if(test_name()) { \
        printf("Result: PASSED\n"); \
    } else { \
        printf("Result: FAILED\n"); \
        test_failed++; \
    } \
    test_run++; \
} while (0)



#endif //TESTS_H