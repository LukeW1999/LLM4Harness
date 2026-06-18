#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;
    /* maximum length for the string data */
    const size_t MAX_LEN = 256;
    uint8_t storage[sizeof(struct aws_string) + MAX_LEN];

    /* nondeterministically decide whether we have a string or NULL */
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_LEN);

        str = (struct aws_string *)storage;

        /* nondeterministically choose an allocator (default or NULL) */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        str->len = len;

        /* initialize the flexible array member with nondet data */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
    } else {
        str = NULL;
    }

    /* assume the string is valid before the call */
    __CPROVER_assume(aws_string_is_valid(str));

    /* save immutable fields for later comparison */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    if (str) {
        old_allocator = str->allocator;
        old_len = str->len;
    }

    /* call the function under test */
    aws_string_destroy_secure(str);

    /* post‑condition checks */
    if (str) {
        /* unchanged fields */
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);

        if (old_allocator == NULL) {
            /* when there is no allocator the memory is not freed;
               the function must have zeroed the data bytes */
            for (size_t i = 0; i < old_len; ++i) {
                assert(((uint8_t *)str->bytes)[i] == 0);
            }
            /* the string must still be valid because it was not deallocated */
            assert(aws_string_is_valid(str));
        } else {
            /* when an allocator is present the memory may have been released.
               No further assertions on the bytes are made. */
        }
    } else {
        /* str == NULL: the function is a no‑op, nothing to assert. */
    }
}
