#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MATRIX_IMPLEMENTATION
#include "matrix.h"

// Small helper macros

#define TEST_START(name_str) \
    int failed = 0;          \
    fputs("Running test: " name_str "\n", stderr);

#define TEST_END                    \
    if (!failed)                    \
        fputs("    ✅\n\n", stderr); \
    else                            \
        fputc('\n', stderr);        \
    return failed;

#define TEST_FAIL_PREFIX "    ❌ "

#define TEST_DEQ(msg, expected, got)                                          \
    if ((expected) != (got)) {                                                \
        fprintf(stderr, TEST_FAIL_PREFIX "%s - expected %f, got %f\n", (msg), \
                (expected), (got));                                           \
        failed = 1;                                                           \
    }

#define TEST_SIZE_EQ(msg, expected, got)                                 \
    if ((expected) != (got)) {                                           \
        fprintf(stderr, TEST_FAIL_PREFIX "%s - expected %zu, got %zu\n", \
                (msg), (expected), (got));                               \
        failed = 1;                                                      \
    }

// Test functions

int test_matrix_new_get_set() {
    TEST_START("new/get/set");

    matrix m = matrix_new(3, 4);
    TEST_SIZE_EQ("m.height", 3lu, m.height);
    TEST_SIZE_EQ("m.width", 4lu, m.width);

    matrix_set(&m, 0, 0, 1.0);
    matrix_set(&m, 0, 1, -2.0);
    matrix_set(&m, 1, 3, 4.0);

    double got = matrix_get(&m, 0, 0);
    TEST_DEQ("m[0][0]", 1.0, got);

    got = matrix_get(&m, 0, 1);
    TEST_DEQ("m[0][1]", -2.0, got);

    got = matrix_get(&m, 1, 3);
    TEST_DEQ("m[1][3]", 4.0, got);

    matrix_del(&m);
    TEST_END;
}

int test_matrix_new_zeroed() {
    TEST_START("new_zeroed");

    matrix m = matrix_new_zeroed(2, 1);
    TEST_SIZE_EQ("m.height", 2lu, m.height);
    TEST_SIZE_EQ("m.width", 1lu, m.width);

    TEST_DEQ("m[0][0]", 0.0, m.values[0]);
    TEST_DEQ("m[1][0]", 0.0, m.values[1]);

    matrix_del(&m);
    TEST_END;
}

int test_matrix_new_repeated() {
    TEST_START("new_repeated/fill_scalar");
    // new_repeated uses fill_scalar

    matrix m = matrix_new_repeated(1, 2, 3.14);
    TEST_SIZE_EQ("m.height", 1lu, m.height);
    TEST_SIZE_EQ("m.width", 2lu, m.width);

    TEST_DEQ("m[0][0]", 3.14, m.values[0]);
    TEST_DEQ("m[0][1]", 3.14, m.values[1]);

    matrix_del(&m);
    TEST_END;
}

int test_matrix_new_uniform() {
    TEST_START("new_uniform/fill_uniform");
    // new_uniform uses fill_uniform
    srand(420);  // To makes test reproducible

    matrix m = matrix_new_uniform(10, 10, -2.0, 4.0);
    TEST_SIZE_EQ("m.height", 10lu, m.height);
    TEST_SIZE_EQ("m.width", 10lu, m.width);

    for (size_t i = 0; i < 100; ++i) {
        if (m.values[i] < -2.0 || m.values[i] > 4.0) {
            failed = 1;
            fprintf(stderr,
                    TEST_FAIL_PREFIX "m.values[%zu] - expected %f..%f, got %f",
                    i, -2.0, 4.0, m.values[i]);
        }
    }

    matrix_del(&m);
    TEST_END;
}

int test_matrix_copy() {
    TEST_START("copy/copy_into")
    // copy uses copy_into

    matrix m1 = matrix_new(2, 3);
    m1.values[0] = -2.0;
    m1.values[1] = 1.0;
    m1.values[2] = 1.0;
    m1.values[3] = 4.0;
    m1.values[4] = 3.14;
    m1.values[5] = -0.5;

    matrix m2 = matrix_copy(&m1);
    TEST_SIZE_EQ("m2.height", 2lu, m2.height);
    TEST_SIZE_EQ("m2.width", 3lu, m2.width);

    TEST_DEQ("m2.values[0]", m1.values[0], m2.values[0]);
    TEST_DEQ("m2.values[1]", m1.values[1], m2.values[1]);
    TEST_DEQ("m2.values[2]", m1.values[2], m2.values[2]);
    TEST_DEQ("m2.values[3]", m1.values[3], m2.values[3]);
    TEST_DEQ("m2.values[4]", m1.values[4], m2.values[4]);
    TEST_DEQ("m2.values[5]", m1.values[5], m2.values[5]);

    matrix_del(&m1);
    matrix_del(&m2);
    TEST_END;
}

int test_matrix_len() {
    TEST_START("len")
    matrix m = {.height = 2, .width = 3, .values = NULL};
    TEST_SIZE_EQ("matrix_len(m)", 6lu, matrix_len(&m));
    TEST_END;
}

int test_matrix_add() {
    TEST_START("add");

    double m1_vals[4] = {1.0, 2.0, -1.0, -0.5};
    double m2_vals[4] = {1.0, 0.0, 3.0, 2.5};
    matrix m1 = {2, 2, m1_vals};
    matrix m2 = {2, 2, m2_vals};

    matrix_add(&m1, &m2);

    TEST_DEQ("m1_vals[0]", 2.0, m1_vals[0]);
    TEST_DEQ("m1_vals[1]", 2.0, m1_vals[1]);
    TEST_DEQ("m1_vals[2]", 2.0, m1_vals[2]);
    TEST_DEQ("m1_vals[3]", 2.0, m1_vals[3]);

    TEST_END;
}

int test_matrix_sub() {
    TEST_START("sub");

    double m1_vals[4] = {1.0, 2.0, -1.0, -0.5};
    double m2_vals[4] = {1.0, 0.0, 3.0, 2.5};
    matrix m1 = {2, 2, m1_vals};
    matrix m2 = {2, 2, m2_vals};

    matrix_sub(&m1, &m2);

    TEST_DEQ("m1_vals[0]", 0.0, m1_vals[0]);
    TEST_DEQ("m1_vals[1]", 2.0, m1_vals[1]);
    TEST_DEQ("m1_vals[2]", -4.0, m1_vals[2]);
    TEST_DEQ("m1_vals[3]", -3.0, m1_vals[3]);

    TEST_END;
}

int test_matrix_add_scalar() {
    TEST_START("add_scalar");

    double m_vals[4] = {1.0, 2.0, -1.0, -0.5};
    matrix m = {2, 2, m_vals};

    matrix_add_scalar(&m, 1.5);

    TEST_DEQ("m_vals[0]", 2.5, m_vals[0]);
    TEST_DEQ("m_vals[1]", 3.5, m_vals[1]);
    TEST_DEQ("m_vals[2]", 0.5, m_vals[2]);
    TEST_DEQ("m_vals[3]", 1.0, m_vals[3]);

    TEST_END;
}

int test_matrix_sub_scalar() {
    TEST_START("sub_scalar");

    double m_vals[4] = {1.0, 2.0, -1.0, -0.5};
    matrix m = {2, 2, m_vals};

    matrix_sub_scalar(&m, 1.5);

    TEST_DEQ("m_vals[0]", -0.5, m_vals[0]);
    TEST_DEQ("m_vals[1]", 0.5, m_vals[1]);
    TEST_DEQ("m_vals[2]", -2.5, m_vals[2]);
    TEST_DEQ("m_vals[3]", -2.0, m_vals[3]);

    TEST_END;
}

int test_matrix_mul_scalar() {
    TEST_START("mul_scalar");

    double m_vals[4] = {1.0, 2.0, -1.0, -0.5};
    matrix m = {2, 2, m_vals};

    matrix_mul_scalar(&m, 2.0);

    TEST_DEQ("m_vals[0]", 2.0, m_vals[0]);
    TEST_DEQ("m_vals[1]", 4.0, m_vals[1]);
    TEST_DEQ("m_vals[2]", -2.0, m_vals[2]);
    TEST_DEQ("m_vals[3]", -1.0, m_vals[3]);

    TEST_END;
}

int test_matrix_pow_scalar() {
    TEST_START("pow_scalar");

    double m_vals[4] = {1.0, 2.0, -1.0, 0.5};
    matrix m = {2, 2, m_vals};

    matrix_pow_scalar(&m, 3.0);

    TEST_DEQ("m_vals[0]", 1.0, m_vals[0]);
    TEST_DEQ("m_vals[1]", 8.0, m_vals[1]);
    TEST_DEQ("m_vals[2]", -1.0, m_vals[2]);
    TEST_DEQ("m_vals[3]", 0.125, m_vals[3]);

    TEST_END;
}

int test_matrix_matmul() {
    TEST_START("matmul/matmul_into");
    // matmul uses matmul_into

    double m1_vals[4] = {1.0, 2.0, 3.0, 4.0};
    double m2_vals[2] = {5.0, 6.0};
    matrix m1 = {2, 2, m1_vals};
    matrix m2 = {2, 1, m2_vals};

    matrix m3 = matrix_matmul(&m1, &m2);
    TEST_SIZE_EQ("m3.height", 2lu, m3.height);
    TEST_SIZE_EQ("m3.width", 1lu, m3.width);

    TEST_DEQ("m3.values[0]", 17.0, m3.values[0]);
    TEST_DEQ("m3.values[1]", 39.0, m3.values[1]);

    matrix_del(&m3);
    TEST_END;
}

int test_matrix_transpose_column() {
    TEST_START("transpose_column");

    double m_vals[2] = {1.0, 2.0};
    matrix m = {2, 1, m_vals};

    matrix_transpose(&m);
    TEST_SIZE_EQ("m.height", 1lu, m.height);
    TEST_SIZE_EQ("m.width", 2lu, m.width);

    TEST_DEQ("m.values[0]", 1.0, m.values[0]);
    TEST_DEQ("m.values[1]", 2.0, m.values[1]);

    TEST_END;
}

int test_matrix_transpose_square() {
    TEST_START("transpose_square");

    double m_vals[9] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    matrix m = {3, 3, m_vals};

    matrix_transpose(&m);
    TEST_SIZE_EQ("m.height", 3lu, m.height);
    TEST_SIZE_EQ("m.width", 3lu, m.width);

    TEST_DEQ("m.values[0]", 1.0, m.values[0]);
    TEST_DEQ("m.values[1]", 4.0, m.values[1]);
    TEST_DEQ("m.values[2]", 7.0, m.values[2]);
    TEST_DEQ("m.values[3]", 2.0, m.values[3]);
    TEST_DEQ("m.values[4]", 5.0, m.values[4]);
    TEST_DEQ("m.values[5]", 8.0, m.values[5]);
    TEST_DEQ("m.values[6]", 3.0, m.values[6]);
    TEST_DEQ("m.values[7]", 6.0, m.values[7]);
    TEST_DEQ("m.values[8]", 9.0, m.values[8]);

    TEST_END;
}

int test_matrix_transpose_rectangle() {
    TEST_START("transpose_rectangle");

    double m_vals[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    matrix m = {2, 3, m_vals};

    matrix_transpose(&m);
    TEST_SIZE_EQ("m.height", 3lu, m.height);
    TEST_SIZE_EQ("m.width", 2lu, m.width);

    TEST_DEQ("m.values[0]", 1.0, m.values[0]);
    TEST_DEQ("m.values[1]", 4.0, m.values[1]);
    TEST_DEQ("m.values[2]", 2.0, m.values[2]);
    TEST_DEQ("m.values[3]", 5.0, m.values[3]);
    TEST_DEQ("m.values[4]", 3.0, m.values[4]);
    TEST_DEQ("m.values[5]", 6.0, m.values[5]);

    TEST_END;
}

int test_matrix_transpose_huge_rectangle() {
    TEST_START("transpose_huge_rectangle");
    srand(420);  // To makes test reproducible

    matrix m = matrix_new_uniform(16, 8, -1.0, 1.0);

    matrix_transpose(&m);
    TEST_SIZE_EQ("m.height", 8lu, m.height);
    TEST_SIZE_EQ("m.width", 16lu, m.width);

    TEST_END;
}


double random_linear_func(double x) { return 2.0 * x - 4.0; }

int test_matrix_map() {
    TEST_START("map");

    double m_vals[4] = {1.0, -2.0, 8.0, 0.5};
    matrix m = {2, 2, m_vals};

    matrix_map(&m, random_linear_func);

    TEST_DEQ("m.values[0]", -2.0, m.values[0]);
    TEST_DEQ("m.values[1]", -8.0, m.values[1]);
    TEST_DEQ("m.values[2]", 12.0, m.values[2]);
    TEST_DEQ("m.values[3]", -3.0, m.values[3]);

    TEST_END;
}

// Entry point

int main() {
    int total_tests = 18;
    int failed = 0;

    failed += test_matrix_new_get_set();
    failed += test_matrix_new_zeroed();
    failed += test_matrix_new_repeated();
    failed += test_matrix_new_uniform();
    failed += test_matrix_len();
    failed += test_matrix_copy();
    failed += test_matrix_add();
    failed += test_matrix_sub();
    failed += test_matrix_add_scalar();
    failed += test_matrix_sub_scalar();
    failed += test_matrix_mul_scalar();
    failed += test_matrix_pow_scalar();
    failed += test_matrix_map();
    failed += test_matrix_matmul();
    failed += test_matrix_transpose_column();
    failed += test_matrix_transpose_square();
    failed += test_matrix_transpose_rectangle();
    failed += test_matrix_transpose_huge_rectangle();

    int succeeded = total_tests - failed;
    fprintf(stderr,
            "--- Summary: Total %d tests; %d succeeded, %d failed ---\n",
            total_tests, succeeded, failed);
    return failed ? 1 : 0;
}
