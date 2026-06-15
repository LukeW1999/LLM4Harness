#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len + 1 <= MAX_BUFFER_SIZE); /* room for null terminator */

        uint8_t buffer[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)buffer;

        /* Use the default allocator as required */
        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;

        str->len = len;

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
        bytes[len] = 0; /* null‑terminate */

        /* Call the function under test */
        aws_string_destroy_secure(str);
        /* No further accesses to `str` or `bytes` after destruction */
    } else {
        str = NULL;
        aws_string_destroy_secure(str);
    }
}
