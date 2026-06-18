#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    const uint8_t *bytes;

    if (len == 0 && nondet_bool()) {
        bytes = NULL;
    } else {
        uint8_t *allocated_bytes = malloc(MAX_BUFFER_SIZE);
        __CPROVER_assume(allocated_bytes != NULL);
        bytes = allocated_bytes;
        __CPROVER_assume(AWS_MEM_IS_READABLE(bytes, len));
    }

    const uint8_t *old_bytes = bytes;
    size_t old_len = len;

    struct store_byte_from_buffer old_byte;
    if (len > 0) {
        save_byte_from_array(bytes, len, &old_byte);
    }

    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    assert(result.ptr == (uint8_t *)old_bytes);
    assert(result.len == old_len);

    assert(bytes == old_bytes);
    assert(len == old_len);

    if (old_len > 0) {
        assert_byte_from_buffer_matches(bytes, &old_byte);
    }

    assert(aws_byte_cursor_is_valid(&result));
}
