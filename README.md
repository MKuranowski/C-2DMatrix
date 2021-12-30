# C-2DMatrix

STB-Style, simple, 2D matrix of doubles with a few most useful operations on matrices.


### Quick Start

Download `matrix.h`; `#define MATRIX_IMPLEMENTATION` and `#include "matrix.h"`.  
Link with `-lm` when compiling.

Take a look at the header part of `matrix.h` to see all available functionality.

```c
#include <stdio.h>

#define MATRIX_IMPLEMENTATION
#include "matrix.h"

int main() {
    matrix m1 = matrix_new(2, 1);
    matrix_set(&m1, 0, 0, 1.0);
    matrix_set(&m1, 1, 0, -1.0);

    matrix m2 = matrix_new(1, 2);
    matrix_set(&m1, 0, 0, 2.0);
    matrix_set(&m1, 1, 0, 1.5);

    matrix m3 = matrix_matmul(&m1, &m2);
    printf("m3 size - %zu x %zu\n", m3.height, m3.width);
    matrix_print(&m3, stdout);

    // Output:
    // m3 size - 2 x 2
    // 2.0 1.5
    // -2.0 -1.5

    matrix_del(&m1);
    matrix_del(&m2);
    matrix_del(&m3);
    return 0;
}
```

See <https://github.com/nothings/stb> for more info on the STB-style libraries.

### No dynamic allocations

It is possible to avoid dynamic allocations of matrices altogether.
This is useful if e.g. matrix sizes are known at compile time.

Take a look at `test.c` - most of the function don't use `matrix_new`/`_del`/`_copy`.

For convince, if `MATRIX_NO_MALLOC` is defined, declarations and definitions
for functions using dynamic memory are not provided at all.


### Running tests

```sh
./build_test.sh
./test
```
