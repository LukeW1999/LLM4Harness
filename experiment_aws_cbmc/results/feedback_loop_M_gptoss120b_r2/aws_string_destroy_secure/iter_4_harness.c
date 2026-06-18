#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;
    const size_t MAX_LEN = 256;
    /* Ensure proper alignment for struct aws_string */
    alignas(struct aws_string) uint8_t storage[sizeof(struct aws_string) + MAX_LEN];

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_LEN); /* leave space for null terminator */

        str = (struct aws_string *)storage;
        str->allocator = NULL;               /* use NULL allocator to avoid deallocation */
        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        ((uint8_t *)str->bytes)[len] = '\0';  /* null‑terminate the string */
    } else {
        str = NULL;
    }

    size_t old_len = 0;
    if (str) {
        old_len = str->len;
    }

    aws_string_destroy_secure(str);

    if (str) {
        for (size_t i = 0; i < old_len; ++i) {
            assert(((uint8_t *)str->bytes)[i] == 0);
        }
        /* The string structure should still be valid after secure destroy with NULL allocator */
        assert(str->allocator == NULL);
        assert(str->len == old_len);
    }
}
