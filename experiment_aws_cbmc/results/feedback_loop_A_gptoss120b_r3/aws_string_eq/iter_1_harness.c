#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    /* 1. Declare nondeterministic inputs */
    struct aws_string *a;
    struct aws_string *b;

    /* a may be NULL or a valid string */
    if (nondet_bool()) {
        a = NULL;
    } else {
        size_t len_a = nondet_size_t();
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        a = (struct aws_string *)malloc(sizeof(struct aws_string) + len_a);
        __CPROVER_assume(a != NULL);
        a->allocator = aws_default_allocator();
        a->len = len_a;
        for (size_t i = 0; i < len_a; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(a));
    }

    /* b may be NULL or a valid string */
    if (nondet_bool()) {
        b = NULL;
    } else {
        size_t len_b = nondet_size_t();
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        b = (struct aws_string *)malloc(sizeof(struct aws_string) + len_b);
        __CPROVER_assume(b != NULL);
        b->allocator = aws_default_allocator();
        b->len = len_b;
        for (size_t i = 0; i < len_b; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(b));
    }

    /* 2. Save old state for immutability checks */
    size_t old_len_a = 0, old_len_b = 0;
    uint8_t *old_bytes_a = NULL, *old_bytes_b = NULL;

    if (a != NULL) {
        old_len_a = a->len;
        old_bytes_a = (uint8_t *)malloc(old_len_a);
        __CPROVER_assume(old_bytes_a != NULL);
        for (size_t i = 0; i < old_len_a; ++i) {
            old_bytes_a[i] = a->bytes[i];
        }
    }

    if (b != NULL) {
        old_len_b = b->len;
        old_bytes_b = (uint8_t *)malloc(old_len_b);
        __CPROVER_assume(old_bytes_b != NULL);
        for (size_t i = 0; i < old_len_b; ++i) {
            old_bytes_b[i] = b->bytes[i];
        }
    }

    /* 3. Call function under test */
    bool result = aws_string_eq(a, b);

    /* 4. Postcondition checks */

    /* Equality semantics */
    if (a == b) {
        assert(result);
    } else if (a == NULL || b == NULL) {
        assert(!result);
    } else {
        if (result) {
            /* When true, lengths must match and all bytes must be equal */
            assert(a->len == b->len);
            for (size_t i = 0; i < a->len; ++i) {
                assert(a->bytes[i] == b->bytes[i]);
            }
        } else {
            /* When false, either lengths differ or some byte differs */
            bool all_equal = true;
            if (a->len != b->len) {
                all_equal = false;
            } else {
                for (size_t i = 0; i < a->len; ++i) {
                    if (a->bytes[i] != b->bytes[i]) {
                        all_equal = false;
                        break;
                    }
                }
            }
            assert(!all_equal);
        }
    }

    /* 5. Unchanged fields (immutability) */
    if (a != NULL) {
        assert(a->allocator == aws_default_allocator());
        assert(a->len == old_len_a);
        for (size_t i = 0; i < old_len_a; ++i) {
            assert(a->bytes[i] == old_bytes_a[i]);
        }
    }
    if (b != NULL) {
        assert(b->allocator == aws_default_allocator());
        assert(b->len == old_len_b);
        for (size_t i = 0; i < old_len_b; ++i) {
            assert(b->bytes[i] == old_bytes_b[i]);
        }
    }

    /* 6. Validity invariants */
    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }

    /* Cleanup (optional for CBMC) */
    free(old_bytes_a);
    free(old_bytes_b);
    free(a);
    free(b);
}
