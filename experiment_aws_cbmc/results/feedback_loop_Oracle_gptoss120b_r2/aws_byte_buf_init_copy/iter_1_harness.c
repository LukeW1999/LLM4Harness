#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_harness(void) {
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *alloc = aws_default_allocator();

    /* structural assumptions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* snapshot of src buffer (if any) */
    uint8_t *src_snapshot = NULL;
    if (src.buffer) {
        src_snapshot = malloc(src.capacity);
        __CPROVER_assume(src_snapshot != NULL);
        memcpy(src_snapshot, src.buffer, src.capacity);
    }

    /* call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* return value must be success or error */
    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR,
                     "aws_byte_buf_init_copy return value must be AWS_OP_SUCCESS or AWS_OP_ERR");

    if (!src.buffer) {
        /* when source has no buffer, dest must be zeroed except allocator */
        __CPROVER_assert(dest.buffer == NULL, "dest.buffer is NULL when src.buffer is NULL");
        __CPROVER_assert(dest.len == 0, "dest.len is 0 when src.buffer is NULL");
        __CPROVER_assert(dest.capacity == 0, "dest.capacity is 0 when src.buffer is NULL");
        __CPROVER_assert(dest.allocator == alloc, "dest.allocator is set to provided allocator");
        __CPROVER_assert(result == AWS_OP_SUCCESS, "result is AWS_OP_SUCCESS when src.buffer is NULL");
    } else {
        if (dest.buffer != NULL) {
            /* allocation succeeded */
            __CPROVER_assert(result == AWS_OP_SUCCESS, "result is AWS_OP_SUCCESS on successful allocation");
            __CPROVER_assert(dest.allocator == alloc, "dest.allocator is set to provided allocator");
            __CPROVER_assert(dest.len == src.len, "dest.len equals src.len");
            __CPROVER_assert(dest.capacity == src.capacity, "dest.capacity equals src.capacity");
            __CPROVER_assert(memcmp(dest.buffer, src.buffer, src.len) == 0,
                             "dest.buffer content matches src.buffer for src.len bytes");
            __CPROVER_assert(dest.capacity >= dest.len, "dest.capacity >= dest.len");
        } else {
            /* allocation failed */
            __CPROVER_assert(result == AWS_OP_ERR, "result is AWS_OP_ERR on allocation failure");
            __CPROVER_assert(dest.buffer == NULL, "dest.buffer is NULL on allocation failure");
            __CPROVER_assert(dest.len == 0 && dest.capacity == 0,
                             "dest.len and dest.capacity are zero on allocation failure");
            __CPROVER_assert(dest.allocator == NULL, "dest.allocator is NULL on allocation failure");
        }
    }

    /* frame condition: source buffer must remain unchanged */
    if (src.buffer) {
        __CPROVER_assert(memcmp(src.buffer, src_snapshot, src.capacity) == 0,
                         "src.buffer unchanged after aws_byte_buf_init_copy");
    }

    free(src_snapshot);
    return 0;
}
