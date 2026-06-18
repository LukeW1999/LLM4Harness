#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

/* nondet helpers */
uint8_t nondet_uint8_t(void);
size_t nondet_size_t(void);

void aws_byte_buf_from_array_harness(void) {
    /* symbolic length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* allocate a writable buffer */
    uint8_t *bytes = (uint8_t *)malloc(len);
    __CPROVER_assume(bytes != NULL || len == 0);

    /* nondet contents */
    for (size_t i = 0; i < len; ++i) {
        bytes[i] = nondet_uint8_t();
    }

    /* snapshot original state */
    uint8_t *bytes_snapshot = NULL;
    if (len > 0) {
        bytes_snapshot = (uint8_t *)malloc(len);
        __CPROVER_assume(bytes_snapshot != NULL);
        memcpy(bytes_snapshot, bytes, len);
    }
    size_t len_snapshot = len;

    /* call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_array(bytes, len);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* result fields must reflect the input array */
    assert(result.buffer == bytes);
    assert(result.len == len);
    assert(result.capacity == len);
    assert(result.allocator == NULL);

    /* length/capacity invariant */
    assert(result.len <= result.capacity);

    /* frame condition: input array must be unchanged */
    if (len > 0) {
        for (size_t i = 0; i < len; ++i) {
            assert(bytes[i] == bytes_snapshot[i]);
        }
    }

    /* cleanup */
    free(bytes);
    free(bytes_snapshot);
}
