set -ex

CFLAGS="-std=c11 --pedantic -Wall -Wextra -Werror -ggdb -fsanitize=undefined"
LIBS="-lm"

gcc $CFLAGS test.c -o test $LIBS
