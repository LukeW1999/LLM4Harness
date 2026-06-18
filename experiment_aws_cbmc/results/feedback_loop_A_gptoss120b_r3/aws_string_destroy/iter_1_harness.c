#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;

    /* nondeterministically choose a dynamically allocated string or a static literal */
    if (nondet_bool()) {
        /* dynamically allocated string – allocator is non‑NULL */
        const char *c_str = "dynamic";
        str = aws_string_new_from_c_str(aws_default_allocator(), c_str);
        __CPROVER_assume(str != NULL);
    } else {
        /* static string – allocator is NULL (AWS_STATIC_STRING_FROM_LITERAL sets allocator to NULL) */
        AWS_STATIC_STRING_FROM_LITERAL(static_lit, "static");
        str = (struct aws_string *)static_lit; /* cast away constness for the destroy call */
    }

    /* Save old observable fields (len and allocator) */
    size_t old_len = 0;
    struct aws_allocator *old_allocator = NULL;
    if (str) {
        old_len = str->len;
        old_allocator = str->allocator;
    }

    /* Call the function under test */
    aws_string_destroy(str);

    /* Post‑condition checks */
    if (str == NULL) {
        /* No operation performed – nothing to assert */
    } else if (old_allocator == NULL) {
        /* Static string: destroy is a no‑op, fields must be unchanged */
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert(aws_string_is_valid(str));
    } else {
        /* Dynamically allocated string: memory has been released.
         * The pointer may now be dangling; we must not dereference it.
         * No further field assertions are made for this path. */
    }

    /* In all cases the function must not corrupt global state;
       the validity predicate for a non‑NULL static string still holds. */
    if (str && old_allocator == NULL) {
        assert(aws_string_is_valid(str));
    }
}
