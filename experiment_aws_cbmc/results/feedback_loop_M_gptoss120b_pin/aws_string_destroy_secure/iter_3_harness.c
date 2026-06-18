#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate a buffer on the stack large enough for the struct and its bytes */
        uint8_t buffer[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)buffer;

        /* the string has no allocator (so destroy will not attempt to free) */
        str->allocator = NULL;

        /* set the length */
        str->len = len;

        /* fill the bytes with nondet data */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* Save old immutable state */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    if (str) {
        old_allocator = str->allocator;
        old_len = str->len;
    }

    /* Call the function under verification */
    aws_string_destroy_secure(str);

    /* Post‑condition checks */
    if (str) {
        /* Fields that must remain unchanged */
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);

        /* If the string had no allocator, it was not freed, so we can inspect the bytes */
        if (old_allocator == NULL) {
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < str->len; ++i) {
                assert(bytes[i] == 0);
            }
            /* The bytes pointer itself must stay the same */
            assert((void *)bytes == (void *)aws_string_bytes(str));
        }

        /* The string must still satisfy its validity predicate */
        assert(aws_string_is_valid(str));
    }
}
