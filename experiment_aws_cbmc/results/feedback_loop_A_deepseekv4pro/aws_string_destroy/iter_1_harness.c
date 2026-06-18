#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    /* nondeterministically choose the string to destroy */
    int case_val = nondet_int() % 3; /* 0: NULL, 1: static string, 2: dynamic string */
    struct aws_string *str = NULL;

    if (case_val == 0) {
        str = NULL;
    } else if (case_val == 1) {
        /* static string with no allocator */
        AWS_STATIC_STRING_FROM_LITERAL(static_str, "test");
        str = (struct aws_string *)static_str; /* static_str is a pointer to the static string */
    } else {
        /* dynamic string allocated with default allocator */
        str = aws_string_new_from_c_str(aws_default_allocator(), "test");
        /* assume allocation succeeds to ensure we have a valid string */
        __CPROVER_assume(str != NULL);
    }

    /* precondition: if str is non-NULL, it must be a valid string */
    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* save old state */
    size_t old_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;

    /* call the function under test */
    aws_string_destroy(str);

    /* postconditions */
    if (str != NULL) {
        /* The function does not modify the string's fields, even if it frees memory.
         * Since fields are const, they cannot be modified anyway. */
        assert(str->len == old_len);
        assert(str->allocator == old_allocator);
        /* For static strings (allocator == NULL), the string should remain valid. */
        if (old_allocator == NULL) {
            assert(aws_string_is_valid(str));
        }
        /* For dynamic strings, the memory is freed, but we can still check that the function didn't crash.
         * No further asserts are possible without accessing freed memory, which is UB in reality,
         * but in CBMC we can still check that the fields are unchanged. */
    }
    /* If str was NULL, nothing to assert. */
}
