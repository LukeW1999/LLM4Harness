#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* Declare and bound the source byte cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Use the default allocator (non-null as required) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Destination buffer struct (will be initialized by the function) */
    struct aws_byte_buf dest;

    /* Save source cursor contents to check for immutability */
    struct store_byte_from_buffer src_store;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &src_store);
    }

    /* Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Postcondition: the source cursor must remain valid */
    assert(aws_byte_cursor_is_valid(&src));

    /* source data must be unchanged */
    if (src.len > 0) {
        assert_byte_from_buffer_matches(src.ptr, &src_store);
    }

    /* Check results for both success and failure */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));
        /* dest length and capacity equal src length */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        /* allocator set correctly */
        assert(dest.allocator == allocator);
        if (src.len == 0) {
            assert(dest.buffer == NULL);
        } else {
            /* Buffer must be allocated and contain a copy of src data */
            assert(dest.buffer != NULL);
            assert(memcmp(dest.buffer, src.ptr, src.len) == 0);
        }
    } else {
        /* Failure: allocation error when src.len > 0 */
        /* dest must be zeroed out and still valid */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* Even a zeroed buffer is considered valid */
        assert(aws_byte_buf_is_valid(&dest));
    }
}
