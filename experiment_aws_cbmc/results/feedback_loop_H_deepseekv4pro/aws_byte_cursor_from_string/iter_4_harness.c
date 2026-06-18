#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_cursor_from_string_harness() {
    /* Assume MAX_BUFFER_SIZE is defined by the build system (e.g., 10) */
    uint8_t buf[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    struct aws_string *src = NULL;

    /* nondeterministically choose between NULL and a valid string */
    if (nondet_bool()) {
        src = NULL;
    } else {
        src = (struct aws_string *)buf;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocator may be a default allocator or NULL (static string) */
        /* Use a simple assignment to avoid type conversion issues */
        src->allocator = aws_default_allocator();
        src->len = len;

        /* fill string data with arbitrary bytes, then null-terminate */
        for (size_t i = 0; i < len; i++) {
            src->bytes[i] = nondet_uint8_t();
        }
        src->bytes[len] = 0;

        /* ensure the crafted structure passes aws_string_is_valid */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    size_t old_len = (src != NULL) ? src->len : 0;

    /* function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* postcondition 1: returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&result));

    /* postcondition 2: case split on input value */
    if (src == NULL) {
        assert(result.len == 0);
        assert(result.ptr == NULL);
    } else {
        assert(result.len == src->len);
        /* Explicit cast to avoid type mismatch between uint8_t* and const uint8_t* */
        assert((const uint8_t *)result.ptr == aws_string_bytes(src));

        /* the string itself must remain valid and unchanged */
        assert(aws_string_is_valid(src));
        assert(src->len == old_len);
    }
}
