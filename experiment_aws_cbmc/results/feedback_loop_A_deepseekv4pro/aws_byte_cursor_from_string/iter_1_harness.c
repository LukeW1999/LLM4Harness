#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

void aws_byte_cursor_from_string_harness() {
    /* 1. Declare src and decide between NULL and a valid string. */
    struct aws_string *src;
    bool is_null = nondet_bool();

    if (is_null) {
        src = NULL;
    } else {
        /* Allocate a valid aws_string with bounded length. */
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);

        /* Allocate space for header + data + mandatory trailing '\0'. */
        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        __CPROVER_assume(alloc_size < MAX_BUFFER_SIZE);
        src = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(src != NULL);

        /* Set up the string fields. Use a non-NULL allocator for validity. */
        src->allocator = aws_default_allocator();
        src->len = len;

        /* Fill the data bytes with nondeterministic content. */
        for (size_t i = 0; i < len; i++) {
            src->bytes[i] = nondet_uint8_t();
        }
        /* Ensure the mandatory null terminator immediately after the data. */
        src->bytes[len] = 0;

        /* The constructed string is now valid. */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Call the function under test. */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 3. Assert postconditions for both paths. */
    if (src == NULL) {
        /* NULL → empty (zeroed) cursor. */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* Non-NULL → cursor points to string's bytes, length = string's length. */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    /* 4. Assert validity invariants always hold. */
    assert(aws_byte_cursor_is_valid(&cursor));
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
}
