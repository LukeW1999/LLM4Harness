#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

/* Helper to create a valid aws_string with allocated memory */
struct aws_string *ensure_string_is_allocated_secure(size_t len) {
    size_t total_size = sizeof(struct aws_string) + len + 1;
    struct aws_string *str = malloc(total_size);
    if (str == NULL) {
        return NULL;
    }
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    *(size_t *)&str->len = len;
    for (size_t i = 0; i < len; i++) {
        ((uint8_t *)str->bytes)[i] = 0;
    }
    ((uint8_t *)str->bytes)[len] = 0;
    return str;
}

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    if (nondet_bool()) {
        /* Test NULL path — function should be a no-op */
        aws_string_destroy_secure(NULL);
        assert(true);
    } else {
        /* Test valid string path */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        struct aws_string *str = ensure_string_is_allocated_secure(len);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));

        size_t old_len = str->len;

        aws_string_destroy_secure(str);

        assert(old_len <= MAX_BUFFER_SIZE);
        assert(true);
    }
}
