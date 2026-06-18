#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 32U

void aws_string_eq_harness(void) {
    /* nondeterministic choice whether a and b are NULL */
    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* Allocate and initialize a if not NULL */
    if (!a_is_null) {
        size_t a_len = nondet_size_t();
        __CPROVER_assume(a_len <= MAX_STRING_LEN);
        /* allocate space for len bytes + null terminator */
        a = malloc(sizeof(struct aws_string) + a_len);
        __CPROVER_assume(a != NULL);
        a->allocator = NULL;               /* static string, no allocator */
        a->len = a_len;
        /* nondet fill bytes */
        for (size_t i = 0; i < a_len; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        /* null terminator (not counted in len) */
        a->bytes[a_len] = 0;
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* Allocate and initialize b if not NULL */
    if (!b_is_null) {
        size_t b_len = nondet_size_t();
        __CPROVER_assume(b_len <= MAX_STRING_LEN);
        b = malloc(sizeof(struct aws_string) + b_len);
        __CPROVER_assume(b != NULL);
        b->allocator = NULL;
        b->len = b_len;
        for (size_t i = 0; i < b_len; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
        b->bytes[b_len] = 0;
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* Save old immutable state */
    struct aws_string old_a;
    struct aws_string old_b;
    struct store_byte_from_buffer a_storage;
    struct store_byte_from_buffer b_storage;

    if (a != NULL) {
        old_a = *a;                         /* copy header fields */
        save_byte_from_array(a->bytes, a->len, &a_storage);
    }
    if (b != NULL) {
        old_b = *b;
        save_byte_from_array(b->bytes, b->len, &b_storage);
    }

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition: return value matches specification */
    if (a == b) {
        /* covers case both NULL or same pointer */
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(a->bytes, a->len, b->bytes, b->len);
        assert(result == expected);
    }

    /* Unchanged fields for a */
    if (a != NULL) {
        assert(a->allocator == old_a.allocator);
        assert(a->len == old_a.len);
        assert_byte_from_buffer_matches(a->bytes, &a_storage);
        assert(aws_string_is_valid(a));
    }

    /* Unchanged fields for b */
    if (b != NULL) {
        assert(b->allocator == old_b.allocator);
        assert(b->len == old_b.len);
        assert_byte_from_buffer_matches(b->bytes, &b_storage);
        assert(aws_string_is_valid(b));
    }
}
