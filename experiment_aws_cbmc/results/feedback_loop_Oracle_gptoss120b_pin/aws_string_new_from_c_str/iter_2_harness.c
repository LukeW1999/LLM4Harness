#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* nondet helpers */
size_t nondet_size_t(void);
char nondet_char(void);

void aws_string_new_from_c_str_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondet size for the C string (bounded) */
    size_t max_len = nondet_size_t();
    __CPROVER_assume(max_len < 256);

    /* allocate buffer for c_str */
    char *c_str = malloc(max_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* fill with nondet characters */
    for (size_t i = 0; i < max_len; ++i) {
        c_str[i] = nondet_char();
    }

    /* choose a nondet position for the null terminator */
    size_t null_pos = nondet_size_t();
    __CPROVER_assume(null_pos <= max_len);
    c_str[null_pos] = '\0';

    /* ensure null_pos is the first null byte */
    for (size_t i = 0; i < null_pos; ++i) {
        __CPROVER_assume(c_str[i] != '\0');
    }

    /* make a copy of c_str to check frame condition later */
    char *c_str_copy = malloc(max_len + 1);
    __CPROVER_assume(c_str_copy != NULL);
    for (size_t i = 0; i < max_len + 1; ++i) {
        c_str_copy[i] = c_str[i];
    }

    /* call the function under verification */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* post‑conditions */
    if (result != NULL) {
        /* result must be a valid aws_string */
        assert(aws_string_is_valid(result));

        /* allocator field must be the one we passed */
        assert(result->allocator == allocator);

        /* length must match the actual C‑string length (null_pos) */
        assert(result->len == null_pos);

        /* bytes must equal the original C‑string contents */
        for (size_t i = 0; i < null_pos; ++i) {
            assert(((const uint8_t *)result->bytes)[i] == (uint8_t)c_str[i]);
        }

        /* null terminator immediately after the data */
        assert(((const uint8_t *)result->bytes)[null_pos] == '\0');
    }

    /* frame condition: c_str must be unchanged */
    for (size_t i = 0; i < max_len + 1; ++i) {
        assert(c_str[i] == c_str_copy[i]);
    }
}
