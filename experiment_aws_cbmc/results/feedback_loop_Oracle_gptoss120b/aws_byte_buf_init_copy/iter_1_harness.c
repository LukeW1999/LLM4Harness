#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

/* nondet helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
bool nondet_bool(void);

void aws_byte_buf_init_copy_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* source buffer */
    struct aws_byte_buf src;
    src.capacity = nondet_size_t();
    __CPROVER_assume(src.capacity <= MAX_BUFFER_SIZE);
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= src.capacity);

    src.buffer = NULL;
    if (src.capacity > 0) {
        src.buffer = malloc(src.capacity);
        if (src.buffer) {
            for (size_t i = 0; i < src.capacity; ++i) {
                src.buffer[i] = nondet_uint8_t();
            }
        }
        /* optionally make buffer NULL to hit the null‑source path */
        if (nondet_bool()) {
            free(src.buffer);
            src.buffer = NULL;
        }
    }

    /* structural preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* snapshot source state for frame condition */
    size_t src_len_old   = src.len;
    size_t src_cap_old   = src.capacity;
    uint8_t *src_buf_old = NULL;
    if (src.buffer) {
        src_buf_old = malloc(src.len);
        if (src_buf_old) {
            memcpy(src_buf_old, src.buffer, src.len);
        }
    }

    /* destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* call the function under test */
    int ret = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* ----- postconditions ----- */

    /* 1. return value / error code correctness */
    if (src.buffer == NULL) {
        __CPROVER_assert(ret == AWS_OP_SUCCESS,
                         "aws_byte_buf_init_copy should succeed when src.buffer is NULL");
    } else {
        __CPROVER_assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR,
                         "aws_byte_buf_init_copy must return success or error");
    }

    /* 2. output buffer length/capacity invariants */
    if (src.buffer == NULL) {
        __CPROVER_assert(dest.len == 0, "dest.len == 0 when src.buffer is NULL");
        __CPROVER_assert(dest.capacity == 0, "dest.capacity == 0 when src.buffer is NULL");
        __CPROVER_assert(dest.buffer == NULL, "dest.buffer == NULL when src.buffer is NULL");
        __CPROVER_assert(dest.allocator == alloc, "dest.allocator set to provided allocator");
    } else {
        if (ret == AWS_OP_SUCCESS) {
            __CPROVER_assert(dest.allocator == alloc,
                             "dest.allocator set to provided allocator on success");
            __CPROVER_assert(dest.len == src.len,
                             "dest.len equals src.len on success");
            __CPROVER_assert(dest.capacity == src.capacity,
                             "dest.capacity equals src.capacity on success");
            __CPROVER_assert(dest.buffer != NULL,
                             "dest.buffer allocated on success");
            __CPROVER_assert(memcmp(dest.buffer, src.buffer, src.len) == 0,
                             "dest.buffer contains a copy of src.buffer up to src.len");
        } else { /* AWS_OP_ERR */
            __CPROVER_assert(dest.len == 0, "dest.len == 0 on error");
            __CPROVER_assert(dest.capacity == 0, "dest.capacity == 0 on error");
            __CPROVER_assert(dest.buffer == NULL, "dest.buffer == NULL on error");
            __CPROVER_assert(dest.allocator == NULL, "dest.allocator == NULL on error");
        }
    }

    /* 3. frame conditions (source unchanged) */
    __CPROVER_assert(src.len == src_len_old, "src.len unchanged");
    __CPROVER_assert(src.capacity == src_cap_old, "src.capacity unchanged");
    if (src.buffer && src_buf_old) {
        __CPROVER_assert(memcmp(src.buffer, src_buf_old, src.len) == 0,
                         "src.buffer contents unchanged");
    }

    return 0;
}
