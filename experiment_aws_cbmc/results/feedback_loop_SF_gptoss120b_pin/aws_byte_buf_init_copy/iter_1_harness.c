#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf src;
    struct aws_byte_buf dest;

    /* nondeterministic initialization of src */
    src.len = nondet_size_t();
    src.capacity = nondet_size_t();
    __CPROVER_assume(src.capacity >= src.len);

    if (src.capacity > 0) {
        src.buffer = malloc(src.capacity);
        __CPROVER_assume(src.buffer != NULL);
        for (size_t i = 0; i < src.capacity; ++i) {
            src.buffer[i] = nondet_uint8_t();
        }
    } else {
        src.buffer = NULL;
    }

    /* precondition: src must be a valid byte buffer */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* snapshot of src state */
    size_t src_len = src.len;
    size_t src_cap = src.capacity;
    uint8_t *src_buf = src.buffer;
    uint8_t src_first_byte = 0;
    if (src_len > 0 && src_buf != NULL) {
        src_first_byte = src_buf[0];
    }

    /* call under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* postcondition assertions */
    /* result must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator must be stored */
        assert(dest.allocator == allocator);

        /* length and capacity must match src length */
        assert(dest.len == src_len);
        assert(dest.capacity == src_len);

        /* buffer allocation semantics */
        if (src_len > 0) {
            assert(dest.buffer != NULL);
            /* content must be a copy of src */
            for (size_t i = 0; i < src_len; ++i) {
                assert(dest.buffer[i] == src_buf[i]);
            }
        } else {
            /* zero‑length copy may result in NULL buffer */
            assert(dest.buffer == NULL);
        }
    } else {
        /* on error, dest should be left in a clean state */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        /* allocator may or may not be set; we do not assert it here */
    }

    /* frame conditions: src must be unchanged */
    assert(src.len == src_len);
    assert(src.capacity == src_cap);
    assert(src.buffer == src_buf);
    if (src_len > 0 && src_buf != NULL) {
        assert(src_buf[0] == src_first_byte);
    }
}
