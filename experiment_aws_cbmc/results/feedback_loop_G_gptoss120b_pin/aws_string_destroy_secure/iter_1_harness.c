#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;
    /* nondeterministically decide whether to allocate a string or use NULL */
    if (nondet_bool()) {
        /* allocate a string with a bounded length */
        const size_t MAX_LEN = 256;
        str = malloc(sizeof(struct aws_string) + MAX_LEN - 1);
        __CPROVER_assume(str != NULL);

        /* nondeterministically choose an allocator (NULL or default) */
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }

        /* nondeterministic length bounded by MAX_LEN */
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_LEN);

        /* assume the string satisfies the library’s validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));

        /* save a byte from the original data for later checks */
        struct store_byte_from_buffer old_byte;
        save_byte_from_array(aws_string_bytes(str), str->len, &old_byte);

        /* remember fields that are relevant for post‑condition checks */
        struct aws_allocator *old_allocator = str->allocator;
        size_t old_len = str->len;

        /* call the function under test */
        aws_string_destroy_secure(str);

        /* -----------------------------------------------------------------
         * Post‑condition checks
         * ----------------------------------------------------------------- */

        if (str != NULL) {
            /* The length field never changes */
            assert(str->len == old_len);

            if (old_allocator != NULL) {
                /* When an allocator is present the memory is released.
                 * The object may be freed, so we do not dereference it
                 * after the call. No further state can be observed. */
            } else {
                /* No allocator → memory is not freed.
                 * The data bytes must have been zeroed. */
                assert(aws_string_bytes(str)[old_byte.idx] == 0);
                /* The allocator field remains NULL */
                assert(str->allocator == NULL);
            }

            /* The string must still satisfy the validity predicate
               when it has not been freed. */
            if (old_allocator == NULL) {
                assert(aws_string_is_valid(str));
            }
        } else {
            /* str was NULL on entry; nothing should happen. */
        }
    } else {
        /* str is NULL */
        str = NULL;
        struct aws_string *old_str = str;
        aws_string_destroy_secure(str);
        /* The pointer remains NULL */
        assert(str == old_str);
    }
}
