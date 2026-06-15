#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_from_array_harness(void) {
    struct aws_byte_buf buf;
    const uint8_t *bytes;
    size_t len = nondet_size_t();

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    if (len == 0) {
        bytes = NULL;
    } else {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    uint8_t *src_copy = NULL;
    if (len > 0) {
        src_copy = malloc(len);
        __CPROVER_assume(src_copy != NULL);
        memcpy(src_copy, bytes, len);
    }

    int result = aws_byte_buf_from_array(&buf, bytes, len);

    __CPROVER_assert(result == AWS_OP_SUCCESS, "aws_byte_buf_from_array should succeed");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "resulting buffer should be valid");

    if (len == 0) {
        __CPROVER_assert(buf.buffer == NULL, "buffer should be NULL when length is zero");
    } else {
        __CPROVER_assert(buf.buffer != NULL, "buffer should not be NULL when length > 0");
        __CPROVER_assert(buf.buffer == bytes, "buffer should point to the original array");
    }

    __CPROVER_assert(buf.len == len, "len field should match input length");
    __CPROVER_assert(buf.capacity == len, "capacity field should match input length");
    __CPROVER_assert(buf.allocator == NULL, "allocator should be NULL");

    if (len > 0) {
        assert_bytes_match(buf.buffer, bytes, len);
        assert_bytes_match(bytes, src_copy, len);
    }

    free((void *)bytes);
    free(src_copy);
}
