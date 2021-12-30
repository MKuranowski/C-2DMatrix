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

/**
 * Allocates a new matrix with the provided size.
 * No guarantees are made as to the contents of the matrix.
 *
 * Such matrix needs to be later destroyed with @ref matrix_del
 */
MATRIX_DEF matrix matrix_new(size_t height, size_t width);

/**
 * Allocates a new matrix with the provided size, and then
 * fills every cell with `0.0`.
 *
 * Such matrix needs to be later destroyed with `matrix_del`.
 */
MATRIX_DEF matrix matrix_new_zeroed(size_t height, size_t width);

/**
 * Allocates a new matrix with the provided size, and then
 * fills every cell with `x`.
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
 * Such matrix needs to be later destroyed with `matrix_del`.
 */
MATRIX_DEF matrix matrix_new_uniform(size_t height, size_t width, double a, double b);

/**
 * Deallocates the underlaying dynamic buffer used by a matrix,
 * and sets `m->values = NULL`.
 */
MATRIX_DEF void matrix_del(matrix* m);

/**
 * Returns the number of elements in a matrix (`width * height`).
 */
MATRIX_DEF size_t matrix_len(matrix const* m);

/**
 * Creates a new matrix with the same size and elements as `m`.
 *
 * Returned matrix needs to be later destroyed with `matrix_del`.
 */
MATRIX_DEF matrix matrix_copy(matrix const* m);

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

/**
 * Performs the matrix multiplication of a and b.
 * a's width must be the same as b's height.
 *
 * Returns a newly-allocated matrix of a's height and b's width.
 * The new matrix needs to be then deallocated with `matrix_del`.
 */
MATRIX_DEF matrix matrix_matmul(matrix const* a, matrix const* b);

/**
 * Returns a new matrix that is the result of transposing `m`.
 * Consider using `matrix_copy` with `matrix_transpose`; as that's
 * a much more efficient implementation for smaller matrices.
 *
 * The new matrix needs to be then deallocated with `matrix_del`.
 */
MATRIX_DEF matrix matrix_transposed(matrix* m);

/**
 * In-place version of matrix_transposed.
 *
 * For non-square matrices, that are not column or row vectors
 * (aka. `m->width != m->height && m->width != 1 && m->height != 1`)
 * the size must be at most 64.
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
    size_t end = matrix_len(&m);
    for (size_t i = 0; i < end; ++i) {
        m.values[i] = x;
    }

    return m;
}

MATRIX_DEF matrix matrix_new_uniform(size_t height, size_t width, double a, double b) {
    assert(b > a);
    double len = b - a;
    double r;

    matrix m = matrix_new(height, width);
    size_t end = matrix_len(&m);
    for (size_t i = 0; i < end; ++i) {
        r = (double)rand() / (double)RAND_MAX;  // Generate a random float in range <0, 1>
        m.values[i] = a + r * len;  // Interpolate the random number to be in correct range
    }

    return m;
}

MATRIX_DEF void matrix_del(matrix* m) {
    assert(m && m->values);
    assert(m->values);
    free(m->values);
    m->values = NULL;
}

MATRIX_DEF size_t matrix_len(matrix const* m) {
    assert(m);
    return m->height * m->width;
}

MATRIX_DEF matrix matrix_copy(matrix const* m) {
    assert(m && m->values);
    matrix m2 = matrix_new(m->height, m->width);
    memcpy(m2.values, m->values, sizeof(double) * matrix_len(m));
    return m2;
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

MATRIX_DEF matrix matrix_matmul(matrix const* a, matrix const* b) {
    assert(a && a->values);
    assert(b && b->values);
    assert(a->width == b->height);

    matrix multiplied = matrix_new(a->height, b->width);
    size_t common_len = a->width;

    for (size_t row = 0; row < multiplied.height; ++row) {
        for (size_t col = 0; col < multiplied.width; ++col) {
            size_t field = row * multiplied.width + col;
            multiplied.values[field] = 0.0;

            for (size_t i = 0; i < common_len; ++i) {
                multiplied.values[field] += matrix_get(a, row, i) * matrix_get(b, i, col);
            }
        }
    }

    return multiplied;
}

MATRIX_DEF matrix matrix_transposed(matrix* m) {
    assert(m && m->values);
    matrix transposed = matrix_new(m->width, m->height);

    for (size_t row = 0; row < m->height; ++row) {
        for (size_t col = 0; col < m->width; ++col) {
            matrix_set(
                &transposed,
                col,
                row,
                matrix_get(m, row, col)
            );
        }
    }

    return transposed;
}

// Private helpers for the in-place transpose
MATRIX_DEF uint_fast64_t matrix__set_bit(uint_fast64_t bit_set, uint_fast64_t idx) {
    return bit_set | (1ul << idx);
}

MATRIX_DEF bool matrix__has_bit(uint_fast64_t bit_set, uint_fast64_t idx) {
    return (bit_set & (1ul << idx)) != 0;
}

MATRIX_DEF void matrix__transpose_small_rectangle(matrix* m) {
    uint_fast64_t swapped_set = 0;
    size_t size = matrix_len(m) - 1;

    assert(size < 64); // Only sizes up to 64 elements are supported
    // First and last elements are always in correct places
    // TODO: We could allow sizes up to 66 if we didn't store that info
    // TODO: Maybe with VLAs we could allow arbitrary big matrices?
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
    else
        matrix__transpose_small_rectangle(m);
}


#endif // MATRIX_IMPLEMENTATION
