#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_eq_harness(void) {
    /* Symbolic inputs */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* Allocate and initialize a if not NULL */
    if (!a_is_null) {
        size_t a_len = nondet_size_t();
        __CPROVER_assume(a_len <= 256);
        uint8_t *a_buf = malloc(a_len);
        __CPROVER_assume(a_buf != NULL);
        for (size_t i = 0; i < a_len; ++i) {
            a_buf[i] = nondet_uint8_t();
        }
        a = aws_string_new_from_array(aws_default_allocator(), a_buf, a_len);
        __CPROVER_assume(a != NULL);
        free(a_buf);
    }

    /* Allocate and initialize b if not NULL */
    if (!b_is_null) {
        size_t b_len = nondet_size_t();
        __CPROVER_assume(b_len <= 256);
        uint8_t *b_buf = malloc(b_len);
        __CPROVER_assume(b_buf != NULL);
        for (size_t i = 0; i < b_len; ++i) {
            b_buf[i] = nondet_uint8_t();
        }
        b = aws_string_new_from_array(aws_default_allocator(), b_buf, b_len);
        __CPROVER_assume(b != NULL);
        free(b_buf);
    }

    /* Preconditions: strings must be valid if non‑NULL */
    __CPROVER_assume(a_is_null || aws_string_is_valid(a));
    __CPROVER_assume(b_is_null || aws_string_is_valid(b));

    /* Snapshot of input state */
    const struct aws_string *a_snapshot = a;
    const struct aws_string *b_snapshot = b;
    size_t a_len_snapshot = a ? a->len : 0;
    size_t b_len_snapshot = b ? b->len : 0;
    const uint8_t *a_bytes_snapshot = a ? a->bytes : NULL;
    const uint8_t *b_bytes_snapshot = b ? b->bytes : NULL;

    /* Call the function under verification */
    bool result = aws_string_eq(a, b);

    
}
