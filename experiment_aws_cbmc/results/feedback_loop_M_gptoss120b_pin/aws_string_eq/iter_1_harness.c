#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/string.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_string_eq_harness(void) {
    /* 1. Non‑deterministic allocation / nullness of the two strings */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* ----- a ----- */
    if (!a_is_null) {
        size_t a_len = nondet_size_t();
        __CPROVER_assume(a_len <= MAX_BUFFER_SIZE);

        /* allocate space for the struct plus the flexible array */
        a = malloc(sizeof(struct aws_string) + a_len);
        __CPROVER_assume(a != NULL);

        /* initialise fields (cast away constness for the harness) */
        a->allocator = aws_default_allocator();
        *((size_t *)&a->len) = a_len;               /* len is const in the real type */
        /* bytes are left uninitialised – they are nondet, which is fine for the proof */

        /* ensure the string satisfies the library’s validity predicate */
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* ----- b ----- */
    if (!b_is_null) {
        size_t b_len = nondet_size_t();
        __CPROVER_assume(b_len <= MAX_BUFFER_SIZE);

        b = malloc(sizeof(struct aws_string) + b_len);
        __CPROVER_assume(b != NULL);

        b->allocator = aws_default_allocator();
        *((size_t *)&b->len) = b_len;
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* 2. Save old immutable state for later comparison */
    struct aws_string *old_a = a;
    struct aws_string *old_b = b;

    struct store_byte_from_buffer a_bytes_snapshot = {0};
    struct store_byte_from_buffer b_bytes_snapshot = {0};

    if (a != NULL) {
        save_byte_from_array(a->bytes, a->len, &a_bytes_snapshot);
    }
    if (b != NULL) {
        save_byte_from_array(b->bytes, b->len, &b_bytes_snapshot);
    }

    /* 3. Call the function under test */
    bool result = aws_string_eq(a, b);

    /* 4. Compute the expected result using the same logic as the implementation */
    bool expected;
    if (a == b) {
        expected = true;
    } else if (a == NULL || b == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
    }

    /* 5. Assert that the function’s return matches the expected value */
    assert(result == expected);

    /* 6. Assert that inputs are unchanged (immutability) */
    if (a != NULL) {
        assert(a->allocator == old_a->allocator);
        assert(a->len == old_a->len);
        assert_byte_from_buffer_matches(a->bytes, &a_bytes_snapshot);
        assert(aws_string_is_valid(a));
    } else {
        assert(a == NULL);
    }

    if (b != NULL) {
        assert(b->allocator == old_b->allocator);
        assert(b->len == old_b->len);
        assert_byte_from_buffer_matches(b->bytes, &b_bytes_snapshot);
        assert(aws_string_is_valid(b));
    } else {
        assert(b == NULL);
    }
}
