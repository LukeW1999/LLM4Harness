#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

/* bounding constant for string length */
#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    /* 1. Declare pointers for the two strings */
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* nondeterministically decide whether each string is allocated */
    if (nondet_bool()) {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a < MAX_STRING_LEN);
        a = malloc(sizeof(struct aws_string) + len_a);
        __CPROVER_assume(a != NULL);
        a->allocator = aws_default_allocator();
        a->len = len_a;
        /* fill the bytes (including the implicit null terminator) with nondet data */
        for (size_t i = 0; i < len_a + 1; ++i) {
            ((uint8_t *)a->bytes)[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    if (nondet_bool()) {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b < MAX_STRING_LEN);
        b = malloc(sizeof(struct aws_string) + len_b);
        __CPROVER_assume(b != NULL);
        b->allocator = aws_default_allocator();
        b->len = len_b;
        for (size_t i = 0; i < len_b + 1; ++i) {
            ((uint8_t *)b->bytes)[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* 2. Save old state (deep copy of mutable fields) */
    struct aws_string old_a_struct;
    struct store_byte_from_buffer old_a_bytes;
    if (a != NULL) {
        old_a_struct = *a;                     /* copies allocator and len */
        save_byte_from_array(a->bytes, a->len, &old_a_bytes);
    }

    struct aws_string old_b_struct;
    struct store_byte_from_buffer old_b_bytes;
    if (b != NULL) {
        old_b_struct = *b;
        save_byte_from_array(b->bytes, b->len, &old_b_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq(a, b);

    /* 4. Post‑condition: return value based on pointer relationships */
    if (a == b) {
        /* both NULL or same non‑NULL pointer */
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        /* exactly one is NULL */
        assert(result == false);
    } else {
        /* both non‑NULL and distinct: result is whatever aws_array_eq returns;
         * we cannot further characterize it here. */
        (void)result;
    }

    /* 5. Unchanged fields for a */
    if (a != NULL) {
        assert(a->allocator == old_a_struct.allocator);
        assert(a->len == old_a_struct.len);
        assert_byte_from_buffer_matches(a->bytes, &old_a_bytes);
        /* validity must still hold */
        assert(aws_string_is_valid(a));
    }

    /* 6. Unchanged fields for b */
    if (b != NULL) {
        assert(b->allocator == old_b_struct.allocator);
        assert(b->len == old_b_struct.len);
        assert_byte_from_buffer_matches(b->bytes, &old_b_bytes);
        assert(aws_string_is_valid(b));
    }

    return 0;
}
