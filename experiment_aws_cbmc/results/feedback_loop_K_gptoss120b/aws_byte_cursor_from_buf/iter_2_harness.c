#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <aws/common/byte_order.h>
#include <aws/common/common.h>
#include <aws/common/array_list.h>

#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_from_buf_harness(void) {
    /* Use the default allocator for helper functions; the function under test does not allocate. */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Create a nondeterministic, but valid, aws_byte_buf on the stack. */
    struct aws_byte_buf buf;
    make_aws_byte_buf(&buf, allocator);

    /* Record the original state of the buffer for frame checking. */
    struct aws_byte_buf buf_old = buf;
    uint8_t *buffer_old = buf.buffer;

    /* Call the function under verification. */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* ---- Postcondition checks ---- */

    /* Validity of the returned cursor. */
    assert(aws_byte_cursor_is_valid(&cur));

    /* Length relationship. */
    assert(cur.len == buf.len);
    if (buf.len == 0) {
        /* When length is zero the pointer may be NULL. */
        assert(cur.ptr == NULL || cur.ptr == buf.buffer);
    } else {
        assert(cur.ptr == buf.buffer);
    }

    /* ---- Frame condition: buf must be unchanged ---- */
    assert(buf.len == buf_old.len);
    assert(buf.capacity == buf_old.capacity);
    assert(buf.allocator == buf_old.allocator);
    assert(buf.buffer == buffer_old);

    /* The contents of the underlying buffer must not be modified. */
    if (buf.buffer != NULL && buf.len > 0) {
        for (size_t i = 0; i < buf.len; ++i) {
            assert(buf.buffer[i] == ((uint8_t *)buffer_old)[i]);
        }
    }

    /* Clean up the internal buffer allocated by make_aws_byte_buf. */
    if (buf.buffer != NULL) {
        allocator->deallocate(allocator, buf.buffer);
    }
}
