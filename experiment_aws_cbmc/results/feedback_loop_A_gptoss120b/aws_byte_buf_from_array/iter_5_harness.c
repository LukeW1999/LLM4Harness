#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

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

    assert(result == AWS_OP_SUCCESS);
    assert(aws_byte_buf_is_valid(&buf));

    if (len == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer != NULL);
        assert(buf.buffer == bytes);
    }

    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    if (len > 0) {
        assert_bytes_match(buf.buffer, bytes, len);
        assert_bytes_match(bytes, src_copy, len);
    }

    free((void *)bytes);
    free(src_copy);
}
