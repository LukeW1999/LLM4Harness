#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 10

static struct aws_string *make_arbitrary_aws_string_or_null(void) {
    bool is_null;
    __CPROVER_assume(__CPROVER_is_fresh(&is_null, sizeof(is_null)));
    if (is_null) {
        return NULL;
    }

    size_t len;
    __CPROVER_assume(__CPROVER_is_fresh(&len, sizeof(len)));
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Allocate struct + len bytes + 1 null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);

    /* Initialize fields via pointer casting to bypass const */
    struct aws_allocator **alloc_ptr = (struct aws_allocator **)&str->allocator;
    *alloc_ptr = aws_default_allocator();

    size_t *len_ptr = (size_t *)&str->len;
    *len_ptr = len;

    /* bytes are already nondet from malloc; ensure null terminator */
    uint8_t *bytes_ptr = (uint8_t *)str->bytes;
    bytes_ptr[len] = 0;

    return str;
}

void aws_string_eq_harness(void) {
    /* Create two independent strings (possibly NULL) */
    struct aws_string *a = make_arbitrary_aws_string_or_null();
    struct aws_string *b = make_arbitrary_aws_string_or_null();

    /* Preconditions */
    __CPROVER_assume(a == NULL || aws_string_is_valid(a));
    __CPROVER_assume(b == NULL || aws_string_is_valid(b));

    /* Save state before call for frame condition checks */
    size_t old_a_len = (a != NULL) ? a->len : 0;
    size_t old_b_len = (b != NULL) ? b->len : 0;

    bool result = aws_string_eq(a, b);

    /* Postcondition: result is a valid bool */
    assert(result == true || result == false);

    /* Postcondition: same pointer => true */
    if (a == b) {
        assert(result == true);
    }

    /* Postcondition: one NULL, one non-NULL => false */
    if (a == NULL && b != NULL) {
        assert(result == false);
    }
    if (a != NULL && b == NULL) {
        assert(result == false);
    }

    /* Postcondition: both non-NULL, different pointers */
    if (a != NULL && b != NULL && a != b) {
        if (result == true) {
            /* lengths must be equal */
            assert(a->len == b->len);
        }
        if (a->len != b->len) {
            assert(result == false);
        }
    }

    /* Frame condition: lengths not modified */
    if (a != NULL) {
        assert(a->len == old_a_len);
    }
    if (b != NULL) {
        assert(b->len == old_b_len);
    }

    /* Postcondition: aws_string validity preserved */
    assert(a == NULL || aws_string_is_valid(a));
    assert(b == NULL || aws_string_is_valid(b));

    return 0;
}
