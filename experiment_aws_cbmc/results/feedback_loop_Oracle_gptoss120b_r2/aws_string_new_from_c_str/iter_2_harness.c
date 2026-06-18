#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/* CBMC nondeterministic helpers */
extern unsigned int nondet_uint(void);
extern char nondet_char(void);

void aws_string_new_from_c_str_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondet size for the C string (including space for the terminating NUL) */
    size_t max_len = nondet_uint();
    __CPROVER_assume(max_len < 20);               /* keep the buffer reasonably small */

    /* allocate the C string */
    char *c_str = malloc(max_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* nondet length of the string (number of characters before NUL) */
    size_t str_len = nondet_uint();
    __CPROVER_assume(str_len <= max_len);

    /* fill the string with nondet data (non‑zero) and terminate it */
    for (size_t i = 0; i < str_len; ++i) {
        c_str[i] = nondet_char();
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[str_len] = '\0';

    /* make a copy of the original C string to check frame condition */
    char *c_str_orig = malloc(str_len + 1);
    __CPROVER_assume(c_str_orig != NULL);
    memcpy(c_str_orig, c_str, str_len + 1);

    /* call the function under verification */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* postconditions */
    if (result != NULL) {
        /* result must be a valid aws_string */
        assert(aws_string_is_valid(result));

        /* allocator stored in the string must be the one we passed */
        assert(result->allocator == allocator);

        /* length must match the original C string length */
        assert(result->len == str_len);

        /* the bytes must be identical to the original C string (excluding the NUL) */
        assert(memcmp(result->bytes, c_str, str_len) == 0);

        /* the string must be NUL‑terminated after the data bytes */
        assert(result->bytes[str_len] == '\0');
    }

    /* frame condition: the input C string must not be modified */
    assert(memcmp(c_str, c_str_orig, str_len + 1) == 0);

    /* clean up */
    free(c_str);
    free(c_str_orig);
    if (result != NULL) {
        aws_string_destroy(result);
    }
}
