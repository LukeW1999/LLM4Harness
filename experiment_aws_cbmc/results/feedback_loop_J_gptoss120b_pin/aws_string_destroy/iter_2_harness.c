#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_harness(void) {
    /* 1. Declare a possibly NULL aws_string pointer */
    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* Use a stack‑allocated struct for the string */
        static struct aws_string str_obj;
        str = &str_obj;

        const size_t max_len = 16;                     /* arbitrary bound */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        str->len = len;

        /* nondeterministically decide whether the string has an allocator */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Save old state (only fields we can safely read after a possible free) */
    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;
    size_t old_len = (str != NULL) ? str->len : 0;

    /* 3. Call function under test */
    aws_string_destroy(str);

    /* 4. Post‑condition checks */

    /* The pointer value itself must remain unchanged */
    assert(str == old_str);

    if (str == NULL || old_allocator == NULL) {
        /* No deallocation happened – the struct must be unchanged */
        if (str != NULL) {
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
            /* Validity must still hold */
            assert(aws_string_is_valid(str));
        }
    } else {
        /* str->allocator was non‑NULL, memory may have been released.
         * We cannot dereference str after this point, but the pointer value
         * itself is still the same (as asserted above). */
        /* No further assertions on the contents are made. */
    }
}
