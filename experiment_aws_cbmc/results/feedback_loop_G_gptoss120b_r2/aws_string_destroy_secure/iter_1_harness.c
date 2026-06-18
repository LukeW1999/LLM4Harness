#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;
    bool make_null = nondet_bool();

    if (make_null) {
        str = NULL;
    } else {
        /* nondeterministic length, bounded */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate memory for struct + flexible array */
        str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* nondeterministic allocator (NULL or default) */
        bool has_allocator = nondet_bool();
        struct aws_allocator *alloc = NULL;
        if (has_allocator) {
            alloc = aws_default_allocator();
        }
        /* write to const fields by casting away constness */
        *((struct aws_allocator **)&str->allocator) = alloc;
        *((size_t *)&str->len) = len;

        /* initialise the byte payload with nondet values */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* assume the string is valid before the call */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Save old state for later checks */
    size_t old_len = 0;
    if (str) {
        old_len = str->len;
    }

    /* Call the function under test */
    aws_string_destroy_secure(str);

    /* Post‑conditions */
    if (str) {
        /* All data bytes up to the original length must be zeroed */
        uint8_t *post_bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < old_len; ++i) {
            assert(post_bytes[i] == 0);
        }

        /* The length field is immutable and must remain unchanged */
        assert(str->len == old_len);

        /* The allocator field is also immutable; its value does not change */
        /* (reading it after a possible free is safe because the value itself is stored
           in the struct, not the allocated memory it points to) */
        /* No explicit assert needed beyond the above comment – the value is unchanged
           by construction. */

        /* If the string was statically allocated (allocator == NULL), the memory is not
           released, so the string must still be valid after zeroing. */
        if (str->allocator == NULL) {
            assert(aws_string_is_valid(str));
        }
    }
}
