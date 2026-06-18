#include <stdbool.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* nondet decide if a is NULL */
    if (!nondet_bool()) {
        size_t len_a = (size_t)nondet_uint64_t();
        __CPROVER_assume(len_a <= MAX_STRING_LEN);
        uint8_t a_storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        a = (struct aws_string *)a_storage;
        a->allocator = allocator;
        a->len = len_a;
        for (size_t i = 0; i < len_a; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
    }

    /* nondet decide if b is NULL */
    if (!nondet_bool()) {
        size_t len_b = (size_t)nondet_uint64_t();
        __CPROVER_assume(len_b <= MAX_STRING_LEN);
        uint8_t b_storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        b = (struct aws_string *)b_storage;
        b->allocator = allocator;
        b->len = len_b;
        for (size_t i = 0; i < len_b; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
    }

    /* Save old state */
    size_t old_a_len = 0;
    struct aws_allocator *old_a_alloc = NULL;
    if (a != NULL) {
        old_a_len = a->len;
        old_a_alloc = a->allocator;
    }

    size_t old_b_len = 0;
    struct aws_allocator *old_b_alloc = NULL;
    if (b != NULL) {
        old_b_len = b->len;
        old_b_alloc = b->allocator;
    }

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postconditions */
    if (a == b) {
        __CPROVER_assert(result == true, "result must be true when a == b");
    } else if (a == NULL || b == NULL) {
        __CPROVER_assert(result == false, "result must be false when one is NULL");
    } else {
        bool manual_eq = true;
        if (a->len != b->len) {
            manual_eq = false;
        } else {
            for (size_t i = 0; i < a->len; ++i) {
                if (a->bytes[i] != b->bytes[i]) {
                    manual_eq = false;
                    break;
                }
            }
        }
        __CPROVER_assert(result == manual_eq, "result must match manual equality");
    }

    /* Unchanged fields */
    if (a != NULL) {
        __CPROVER_assert(a->len == old_a_len, "a->len unchanged");
        __CPROVER_assert(a->allocator == old_a_alloc, "a->allocator unchanged");
    }
    if (b != NULL) {
        __CPROVER_assert(b->len == old_b_len, "b->len unchanged");
        __CPROVER_assert(b->allocator == old_b_alloc, "b->allocator unchanged");
    }
}
