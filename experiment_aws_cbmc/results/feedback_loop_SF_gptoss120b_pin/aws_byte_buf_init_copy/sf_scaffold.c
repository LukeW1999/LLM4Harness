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

    /* ASSERT_POSTCONDITIONS_HERE */
}
