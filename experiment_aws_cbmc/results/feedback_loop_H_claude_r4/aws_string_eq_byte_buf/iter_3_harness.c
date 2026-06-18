#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 8
#endif

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void harness(void) {
    /* Set up aws_string */
    const struct aws_string *str = make_arbitrary_aws_string_nondet_len();

    /* Set up aws_byte_buf */
    struct aws_byte_buf buf_storage;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf_storage, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf_storage);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf_storage));
    struct aws_byte_buf *buf = &buf_storage;

    /* Save old state for immutability checks */
    const struct aws_string *old_str = str;
    struct aws_byte_buf old_buf_storage = buf_storage;

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postconditions */
    if (str != NULL && buf != NULL) {
        if (str->len != buf->len) {
            assert(result == false);
        }
    }

    /* Immutability: str pointer unchanged */
    assert(str == old_str);

    /* Immutability: buf fields unchanged */
    assert(buf->len == old_buf_storage.len);
    assert(buf->capacity == old_buf_storage.capacity);
    assert(buf->buffer == old_buf_storage.buffer);
    assert(buf->allocator == old_buf_storage.allocator);

    /* Validity invariants still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    assert(aws_byte_buf_is_valid(buf));
}
