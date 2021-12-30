/**
 * STB-Style, simple, dynamically-allocated 2D matrix of doubles;
 * with a few most useful operations on matrices.
 *
 * SPDX-License-Identifier: WTFPL
 *
 * @author Mikołaj Kuranowski
 * @version 1.0.0
 * @copyright Copyright (c) 2021, Mikolaj Kuranowski
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>  // for size_t
#include <stdio.h>   // for FILE*

#ifndef MATRIX_DEF
#define MATRIX_DEF static inline
#endif // MATRIX_DEF

/**
 * Represents a dynamically-allocated matrix.
 *
 * @property height - number of rows
 * @property width - number of columns
 * @property values - pointer to the allocated array (matrix)
 */
typedef struct {
    size_t height;
    size_t width;
    double* values;
} matrix;

#ifndef MATRIX_NO_MALLOC

/**
 * Allocates a new matrix with the provided size.
 * No guarantees are made as to the contents of the matrix.
 *
 * Such matrix needs to be later destroyed with `matrix_del`.
 */
MATRIX_DEF matrix matrix_new(size_t height, size_t width);

/**
 * Allocates a new matrix with the provided size, and then
 * fills every cell with `0.0`.
 *
 * On some platforms this function might be faster than
 * a `matrix_new` followed by `matrix_fill_scalar` - as
 * `matrix_new_zeroed` uses calloc.
 *
 * Such matrix needs to be later destroyed with `matrix_del`.
 */
MATRIX_DEF matrix matrix_new_zeroed(size_t height, size_t width);

/**
 * Allocates a new matrix with the provided size, and then
 * fills every cell with `x`.
 *
 * Equivalent to `matrix_new `followed by `matrix_fill_scalar`.
 *
 * Such matrix needs to be later destroyed with `matrix_del`.
 */
MATRIX_DEF matrix matrix_new_repeated(size_t height, size_t width, double x);

/**
 * Allocates a new matrix with the provided size, and then
 * fills every cell with a random value between `a` and `b`.
 *
 * This function uses `rand()` internally, so make sure
 * to initialize the random seed with `srand()` beforehand!
 *
 * Equivalent to `matrix_new `followed by `matrix_fill_uniform`.
 *
 * Such matrix needs to be later destroyed with `matrix_del`.
 */
MATRIX_DEF matrix matrix_new_uniform(size_t height, size_t width, double a, double b);

/**
 * Deallocates the underlaying dynamic buffer used by a matrix,
 * and sets `m->values = NULL`.
 */
MATRIX_DEF void matrix_del(matrix* m);

/**
 * Creates a new matrix with the same size and elements as `m`.
 *
 * Returned matrix needs to be later destroyed with `matrix_del`.
 */
MATRIX_DEF matrix matrix_copy(matrix const* m);

#endif  // MATRIX_NO_MALLOC

/**
 * Returns the number of elements in a matrix (`width * height`).
 */
MATRIX_DEF size_t matrix_len(matrix const* m);

/**
 * Dumps the matrix into a sink.
 * Rows are separated by a '\n'; columns by a ' '.
 */
MATRIX_DEF void matrix_print(matrix const* m, FILE* sink);

/**
 * Retrieves the value from a particular cell of the matrix `m`.
 * Shorthand for `m->values[row * m->width + col]`
 */
MATRIX_DEF double matrix_get(matrix const* m, size_t row, size_t col);

/**
 * Sets a value of a particular cell of the matrix `m`.
 * Shorthand for `m->values[row * m->width + col] = value`
 */
MATRIX_DEF void matrix_set(matrix const* m, size_t row, size_t col, double value);

/**
 * Replaces every cell of matrix `m` with `value`.
 */
MATRIX_DEF void matrix_fill_scalar(matrix* m, double value);

/**
 * Replaces every cell of matrix `m` with a random value between `a` and `b`.
 *
 * This function uses `rand()` internally, so make sure
 * to initialize the random seed with `srand()` beforehand!
 */
MATRIX_DEF void matrix_fill_uniform(matrix* m, double a, double b);

/**
 * Performs element-wise addition of b into a,
 * such that ` a_ij = a_ij + b_ij`.
 */
MATRIX_DEF void matrix_add(matrix* a, matrix const* b);

/**
 * Performs element-wise subtraction of b from a,
 * such that ` a_ij = a_ij - b_ij`.
 */
MATRIX_DEF void matrix_sub(matrix* a, matrix const* b);

/**
 * Adds a scalar to every cell of a matrix,
 * such that `a_ij = a_ij + b`.
 */
MATRIX_DEF void matrix_add_scalar(matrix* a, double b);

/**
 * Adds a scalar to every cell of a matrix,
 * such that `a_ij = a_ij + b`.
 */
MATRIX_DEF void matrix_sub_scalar(matrix* a, double b);

/**
 * Multiplies every cell of the matrix by a scalar,
 * such that `a_ij = a_ij * b`.
 */
MATRIX_DEF void matrix_mul_scalar(matrix* a, double b);

/**
 * Raises every cell of the matrix to a scalar,
 * such that `a_ij = pow(a_ij, b)`.
 */
MATRIX_DEF void matrix_pow_scalar(matrix* a, double b);

/**
 * Maps a function to every cell of the matrix,
 * such that `m_ij = func(m_ij)`.
 */
MATRIX_DEF void matrix_map(matrix* m, double(*func)(double));

#ifndef MATRIX_NO_MALLOC

/**
 * Performs the matrix multiplication of a and b.
 * a's width must be the same as b's height.
 *
 * Returns a newly-allocated matrix of a's height and b's width.
 * The new matrix needs to be then deallocated with `matrix_del`.
 */
MATRIX_DEF matrix matrix_matmul(matrix const* a, matrix const* b);

#endif  // MATRIX_NO_MALLOC

/**
 * Performs the matrix multiplication of a and b.
 * a's width must be the same as b's height,
 * dest's height must be the same as a's height and
 * dest's width must be the same as b's width.
 *
 * This function is useful if you want to avoid malloc completely.
 */
MATRIX_DEF void matrix_matmul_into(matrix const* a, matrix const* b, matrix* dest);

/**
 * Transposes the matrix in-place.
 */
MATRIX_DEF void matrix_transpose(matrix* m);

#endif  // MATRIX_H
#ifdef MATRIX_IMPLEMENTATION

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MATRIX_NO_MALLOC

MATRIX_DEF matrix matrix_new(size_t height, size_t width) {
    matrix m;
    m.height = height;
    m.width = width;
    m.values = malloc(sizeof(double) * height * width);
    assert(m.values);
    return m;
}

MATRIX_DEF matrix matrix_new_zeroed(size_t height, size_t width) {
    matrix m;
    m.height = height;
    m.width = width;
    m.values = calloc(height * width, sizeof(double));
    assert(m.values);
    return m;
}

MATRIX_DEF matrix matrix_new_repeated(size_t height, size_t width, double x) {
    matrix m = matrix_new(height, width);
    matrix_fill_scalar(&m, x);
    return m;
}

MATRIX_DEF matrix matrix_new_uniform(size_t height, size_t width, double a, double b) {
    matrix m = matrix_new(height, width);
    matrix_fill_uniform(&m, a, b);
    return m;
}

MATRIX_DEF void matrix_del(matrix* m) {
    assert(m && m->values);
    assert(m->values);
    free(m->values);
    m->values = NULL;
}

MATRIX_DEF matrix matrix_copy(matrix const* m) {
    assert(m && m->values);
    matrix m2 = matrix_new(m->height, m->width);
    memcpy(m2.values, m->values, sizeof(double) * matrix_len(m));
    return m2;
}

#endif  // MATRIX_NO_MALLOC

MATRIX_DEF size_t matrix_len(matrix const* m) {
    assert(m);
    return m->height * m->width;
}

MATRIX_DEF void matrix_print(matrix const* m, FILE* sink) {
    assert(m && m->values);
    for (size_t row = 0; row < m->height; ++row) {
        for (size_t col = 0; col < m->width; ++col) {
            fprintf(sink, "%f ", matrix_get(m, row, col));
        }
        fputc('\n', sink);
    }
}

MATRIX_DEF double matrix_get(matrix const* m, size_t row, size_t col) {
    assert(m && m->values);
    assert(row < m->height);
    assert(col < m->width);

    return m->values[row * m->width + col];
}

MATRIX_DEF void matrix_set(matrix const* m, size_t row, size_t col, double value) {
    assert(m && m->values);
    assert(row < m->height);
    assert(col < m->width);

    m->values[row * m->width + col] = value;
}

MATRIX_DEF void matrix_fill_scalar(matrix* m, double value) {
    size_t end = matrix_len(m);
    for (size_t i = 0; i < end; ++i) {
        m->values[i] = value;
    }
}

MATRIX_DEF void matrix_fill_uniform(matrix* m, double a, double b) {
    assert(b > a);
    double len = b - a;
    double r;

    size_t end = matrix_len(m);
    for (size_t i = 0; i < end; ++i) {
        r = (double)rand() / (double)RAND_MAX;  // Generate a random float in range <0, 1>
        m->values[i] = a + r * len;  // Interpolate the random number to be in correct range
    }
}

MATRIX_DEF void matrix_add(matrix* a, matrix const* b) {
    assert(a && a->values);
    assert(b && b->values);
    assert(a->height == b->height);
    assert(a->width == b->width);
    size_t end = matrix_len(a);

    for (size_t i = 0; i < end; ++i)
        a->values[i] += b->values[i];
}

MATRIX_DEF void matrix_sub(matrix* a, matrix const* b) {
    assert(a && a->values);
    assert(b && b->values);
    assert(a->height == b->height);
    assert(a->width == b->width);
    size_t end = matrix_len(a);

    for (size_t i = 0; i < end; ++i)
        a->values[i] -= b->values[i];
}

MATRIX_DEF void matrix_add_scalar(matrix* a, double b) {
    assert(a && a->values);
    size_t end = matrix_len(a);

    for (size_t i = 0; i < end; ++i)
        a->values[i] += b;
}

MATRIX_DEF void matrix_sub_scalar(matrix* a, double b) {
    assert(a && a->values);
    size_t end = matrix_len(a);

    for (size_t i = 0; i < end; ++i)
        a->values[i] -= b;
}

MATRIX_DEF void matrix_mul_scalar(matrix* a, double b) {
    assert(a && a->values);
    size_t end = matrix_len(a);

    for (size_t i = 0; i < end; ++i)
        a->values[i] *= b;
}

MATRIX_DEF void matrix_pow_scalar(matrix* a, double b) {
    assert(a && a->values);
    size_t end = matrix_len(a);

    for (size_t i = 0; i < end; ++i)
        a->values[i] = pow(a->values[i], b);
}

MATRIX_DEF void matrix_map(matrix* m, double(*func)(double)) {
    assert(m && m->values);
    size_t end = matrix_len(m);

    for (size_t i = 0; i < end; ++i) {
        m->values[i] = func(m->values[i]);
    }
}

#ifndef MATRIX_NO_MALLOC

MATRIX_DEF matrix matrix_matmul(matrix const* a, matrix const* b) {
    assert(a && a->values);
    assert(b && b->values);
    assert(a->width == b->height);

    matrix multiplied = matrix_new(a->height, b->width);
    matrix_matmul_into(a, b, &multiplied);

    return multiplied;
}

#endif  // MATRIX_NO_MALLOC

MATRIX_DEF void matrix_matmul_into(matrix const* a, matrix const* b, matrix* dest) {
    assert(a && a->values);
    assert(b && b->values);
    assert(dest && dest->values);
    assert(a->width == b->height);
    assert(dest->height == a->height);
    assert(dest->width == b->width);

    size_t common_len = a->width;

    for (size_t row = 0; row < dest->height; ++row) {
        for (size_t col = 0; col < dest->width; ++col) {
            size_t field = row * dest->width + col;
            dest->values[field] = 0.0;

            for (size_t i = 0; i < common_len; ++i) {
                dest->values[field] += matrix_get(a, row, i) * matrix_get(b, i, col);
            }
        }
    }
}

// Private helpers for the in-place transpose

/// Returns the number with idx'th bit set
MATRIX_DEF uint64_t matrix__set_bit(uint64_t number, uint64_t idx) {
    return number | ((uint64_t)1 << idx);
}

/// Checks whether idx'th bit is set in the number
MATRIX_DEF bool matrix__has_bit(uint64_t number, uint64_t idx) {
    return (number & ((uint64_t)1 << idx)) != 0;
}

/// Sets the idx'th element of the bitset.
/// Does not check bounds!
MATRIX_DEF void matrix__bitset_set(uint64_t* bitset, uint64_t idx) {
    uint64_t h = idx / 64;
    uint64_t l = idx % 64;
    bitset[h] |= ((uint64_t)1 << l);
}

/// Checks the idx'th element of the bitset.
/// Does not check bounds!
MATRIX_DEF bool matrix__bitset_has(uint64_t* bitset, uint64_t idx) {
    uint64_t h = idx / 64;
    uint64_t l = idx % 64;
    return (bitset[h] & ((uint64_t)1 << l)) != 0;
}

MATRIX_DEF void matrix__transpose_rectangle(matrix* m) {
    size_t size = matrix_len(m) - 1;

    // Initialize the bitset for storing indicies which were already swapped
    size_t bitset_sz = (size / 64) + 1;
    uint64_t bitset[bitset_sz];
    memset(bitset, 0, sizeof(uint64_t) * bitset_sz);

    // Other used attributes
    size_t i = 1;
    size_t cycle_begin;
    size_t next;
    double temp;
    double swap_temp;

    while (i < size) {
        cycle_begin = i;
        temp = m->values[i];

        do {
            next = (i * m->height) % size;

            // Swap next with temp
            swap_temp = m->values[next];
            m->values[next] = temp;
            temp = swap_temp;

            matrix__bitset_set(bitset, i);
            i = next;
        } while (i != cycle_begin);

        // Find the next cycle
        for (i = 1; i < size && matrix__bitset_has(bitset, i); ++i);
    }

    // Swap width and height
    i = m->height;
    m->height = m->width;
    m->width = i;
}

MATRIX_DEF void matrix__transpose_small_rectangle(matrix* m) {
    uint64_t swapped_set = 0;
    size_t size = matrix_len(m) - 1;

    assert(size < 64);
    swapped_set = matrix__set_bit(swapped_set, 0);
    swapped_set = matrix__set_bit(swapped_set, size);

    size_t i = 1;
    size_t cycle_begin;
    size_t next;
    double temp;
    double swap_temp;

    while (i < size) {
        cycle_begin = i;
        temp = m->values[i];

        do {
            next = (i * m->height) % size;

            // Swap next with temp
            swap_temp = m->values[next];
            m->values[next] = temp;
            temp = swap_temp;

            swapped_set = matrix__set_bit(swapped_set, i);
            i = next;
        } while (i != cycle_begin);

        // Find the next cycle
        for (i = 1; i < size && matrix__has_bit(swapped_set, i); ++i);
    }

    // Swap width and height
    i = m->height;
    m->height = m->width;
    m->width = i;
}

MATRIX_DEF void matrix__transpose_square(matrix* m) {
    assert(m->width == m->height);
    double temp;

    for (size_t i = 0; i < m->height - 1; ++i) {
        for (size_t j = i + 1; j < m->height; ++j) {
            temp = matrix_get(m, i, j);
            matrix_set(m, i, j, matrix_get(m, j, i));
            matrix_set(m, j, i, temp);
        }
    }
}

MATRIX_DEF void matrix__transpose_single_col_or_row(matrix* m) {
    assert(m->width == 1 || m->height == 1);

    // Swap width and height
    int temp = m->height;
    m->height = m->width;
    m->width = temp;
}

MATRIX_DEF void matrix_transpose(matrix* m) {
    assert(m && m->values);

    if (m->width == 1 || m->height == 1)
        matrix__transpose_single_col_or_row(m);
    else if (m->width == m->height)
        matrix__transpose_square(m);
    else if (matrix_len(m) <= 64)
        matrix__transpose_small_rectangle(m);
    else
        matrix__transpose_rectangle(m);
}

#endif // MATRIX_IMPLEMENTATION
