#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STR_LEN 20

void aws_string_new_from_c_str_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* allocate and nondet-initialize c_str */
    char *c_str = malloc(MAX_STR_LEN);
    __CPROVER_assume(c_str != NULL);

    /* nondet fill */
    for (size_t i = 0; i < MAX_STR_LEN; ++i) {
        c_str[i] = (char) nondet_uint8_t();
    }

    /* ensure there is a null terminator somewhere inside the buffer */
    size_t null_pos = nondet_uint8_t() % MAX_STR_LEN;
    c_str[null_pos] = '\0';

    /* snapshot original c_str for frame condition */
    char *c_str_old = malloc(MAX_STR_LEN);
    __CPROVER_assume(c_str_old != NULL);
    for (size_t i = 0; i < MAX_STR_LEN; ++i) {
        c_str_old[i] = c_str[i];
    }

    /* call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* postcondition 1: result is either NULL or a valid aws_string */
    assert(result == NULL || aws_string_is_valid(result));

    if (result != NULL) {
        /* compute length of c_str */
        size_t c_len = 0;
        while (c_str[c_len] != '\0') {
            ++c_len;
        }

        /* length invariant */
        assert(result->len == c_len);

        /* content invariant */
        for (size_t i = 0; i < c_len; ++i) {
            assert(((const uint8_t *)result->bytes)[i] == (uint8_t)c_str[i]);
        }

        /* null‑terminator after the bytes (not counted in len) */
        assert(((const uint8_t *)result->bytes)[c_len] == (uint8_t)'\0');
    }

    /* frame condition: c_str must be unchanged */
    for (size_t i = 0; i < MAX_STR_LEN; ++i) {
        assert(c_str[i] == c_str_old[i]);
    }

    /* clean up */
    free(c_str);
    free(c_str_old);
    if (result != NULL) {
        aws_string_destroy(result);
    }

    return 0;
}
