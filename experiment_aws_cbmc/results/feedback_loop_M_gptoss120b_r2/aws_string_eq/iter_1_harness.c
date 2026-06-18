#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_harness(void) {
    /* 1. Non‑deterministic allocation / NULL choice for a */
    bool a_is_null = nondet_bool();
    struct aws_string *a = NULL;
    size_t a_len = 0;
    if (!a_is_null) {
        a_len = nondet_size_t();
        __CPROVER_assume(a_len <= MAX_BUFFER_SIZE);
        /* allocate enough space for the flexible array */
        size_t alloc_size = sizeof(struct aws_string) + a_len;
        a = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(a != NULL);
        a->allocator = aws_default_allocator();
        a->len = a_len;
        for (size_t i = 0; i < a_len; ++i) {
            ((uint8_t *)a->bytes)[i] = nondet_uint8_t();
        }
    }
    __CPROVER_assume(a == NULL || aws_string_is_valid(a));

    /* 2. Non‑deterministic allocation / NULL choice for b */
    bool b_is_null = nondet_bool();
    struct aws_string *b = NULL;
    size_t b_len = 0;
    if (!b_is_null) {
        b_len = nondet_size_t();
        __CPROVER_assume(b_len <= MAX_BUFFER_SIZE);
        size_t alloc_size = sizeof(struct aws_string) + b_len;
        b = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(b != NULL);
        b->allocator = aws_default_allocator();
        b->len = b_len;
        for (size_t i = 0; i < b_len; ++i) {
            ((uint8_t *)b->bytes)[i] = nondet_uint8_t();
        }
    }
    __CPROVER_assume(b == NULL || aws_string_is_valid(b));

    /* 3. Save old immutable state */
    struct aws_allocator *old_a_alloc = NULL;
    size_t old_a_len = 0;
    struct store_byte_from_buffer a_bytes_store;
    if (a != NULL) {
        old_a_alloc = a->allocator;
        old_a_len = a->len;
        save_byte_from_array(a->bytes, a->len, &a_bytes_store);
    }

    struct aws_allocator *old_b_alloc = NULL;
    size_t old_b_len = 0;
    struct store_byte_from_buffer b_bytes_store;
    if (b != NULL) {
        old_b_alloc = b->allocator;
        old_b_len = b->len;
        save_byte_from_array(b->bytes, b->len, &b_bytes_store);
    }

    /* 4. Call function under test */
    bool result = aws_string_eq(a, b);

    /* 5. Compute expected result according to specification */
    bool expected;
    if (a == b) {
        expected = true;
    } else if (a == NULL || b == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
    }
    assert(result == expected);

    /* 6. Unchanged fields – a */
    if (a != NULL) {
        assert(a->allocator == old_a_alloc);
        assert(a->len == old_a_len);
        assert_byte_from_buffer_matches(a->bytes, &a_bytes_store);
    }

    /* 7. Unchanged fields – b */
    if (b != NULL) {
        assert(b->allocator == old_b_alloc);
        assert(b->len == old_b_len);
        assert_byte_from_buffer_matches(b->bytes, &b_bytes_store);
    }

    /* 8. Validity invariants after the call */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }
}
