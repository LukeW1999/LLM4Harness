#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

size_t nondet_size_t(void);
bool nondet_bool(void);

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

    /* Keep a copy of the source data for later comparison */
    uint8_t *src_copy = NULL;
    if (len > 0) {
        src_copy = malloc(len);
        __CPROVER_assume(src_copy != NULL);
        memcpy(src_copy, bytes, len);
    }

    int result = aws_byte_buf_from_array(&buf, bytes, len);

    /* The function should always succeed for the given preconditions */
    assert(result == AWS_OP_SUCCESS);

    /* Verify the resulting buffer fields */
    assert(buf.buffer == bytes);
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));

    /* Verify that the buffer contents match the original data */
    if (len > 0) {
        assert(memcmp(buf.buffer, src_copy, len) == 0);
    }

    free((void *)bytes);
    free(src_copy);
}
