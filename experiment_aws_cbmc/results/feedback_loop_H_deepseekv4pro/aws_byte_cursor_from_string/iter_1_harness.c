#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_cursor_from_string_harness() {
    /* MAX_BUFFER_SIZE is defined by the proof Makefile (e.g., 10) */
    uint8_t buf[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    struct aws_string *src = NULL;

    /* nondeterministically decide if src is NULL or a valid string */
    if (nondet_bool()) {
        src = NULL;
    } else {
        src = (struct aws_string *)buf;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocator can be either a default allocator or NULL (static string) */
        src->allocator = nondet_bool() ? aws_default_allocator() : NULL;
        src->len = len;

        /* fill string data with arbitrary bytes (except the null terminator) */
        for (size_t i = 0; i < len; i++) {
            src->bytes[i] = nondet_uint8_t();
        }
        /* ensure null terminator */
        src->bytes[len] = 0;

        /* assume the constructed string is valid */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* save original length (string is const, but we can still check it) */
    size_t old_len = (src != NULL) ? src->len : 0;

    /* call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* postcondition 1: the returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&result));

    /* postcondition 2: case split on input */
    if (src == NULL) {
        /* NULL src yields an empty cursor */
        assert(result.len == 0);
        assert(result.ptr == NULL);
    } else {
        /* non-null src yields a cursor pointing to the string's data */
        assert(result.len == src->len);
        assert(result.ptr == aws_string_bytes(src));

        /* the string itself must remain valid and unchanged */
        assert(aws_string_is_valid(src));
        assert(src->len == old_len);
    }
}
