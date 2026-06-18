#include <aws/common/string.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_string_harness(void) {
    /* Symbolic input: decide if src is NULL */
    bool src_is_null = nondet_bool();
    struct aws_string *src = NULL;

    if (!src_is_null) {
        /* Symbolic length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* Allocate memory for aws_string with flexible array */
        src = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(src != NULL);

        /* Initialize fields */
        src->allocator = aws_default_allocator();
        src->len = len;

        /* Initialize byte contents */
        for (size_t i = 0; i < len; ++i) {
            src->bytes[i] = nondet_uint8_t();
        }
    }

    /* Precondition: src is either NULL or a valid aws_string */
    __CPROVER_assume(src == NULL || aws_string_is_valid(src));

    /* Snapshot of input state */
    size_t old_len = src ? src->len : 0;
    const uint8_t *old_bytes = src ? src->bytes : NULL;

    /* Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* Postcondition assertions */
    /* The cursor should reference the source string's bytes and length, or be empty if src is NULL */
    assert(cursor.ptr == old_bytes);
    assert(cursor.len == old_len);

    /* Frame conditions: src must remain unchanged */
    if (src != NULL) {
        /* Length unchanged */
        assert(src->len == old_len);
        /* Allocator unchanged */
        assert(src->allocator == aws_default_allocator());
        /* Byte contents unchanged */
        for (size_t i = 0; i < old_len; ++i) {
            assert(src->bytes[i] == old_bytes[i]);
        }
    } else {
        /* When src is NULL, cursor must be empty */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    }
}
