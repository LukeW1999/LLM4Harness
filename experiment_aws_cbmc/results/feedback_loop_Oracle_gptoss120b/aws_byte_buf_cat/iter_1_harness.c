#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_cat_harness(void) {
    /* destination buffer */
    struct aws_byte_buf dest;
    dest.allocator = aws_default_allocator();

    /* nondet capacity and length for dest */
    dest.capacity = nondet_size_t();
    __CPROVER_assume(dest.capacity <= MAX_BUFFER_SIZE);
    dest.len = nondet_size_t();
    __CPROVER_assume(dest.len <= dest.capacity);

    if (dest.capacity > 0) {
        dest.buffer = malloc(dest.capacity);
        __CPROVER_assume(dest.buffer != NULL);
    } else {
        dest.buffer = NULL;
    }

    /* source buffers */
    struct aws_byte_buf buffer1, buffer2, buffer3;
    struct aws_byte_buf *srcs[3] = { &buffer1, &buffer2, &buffer3 };

    for (size_t i = 0; i < 3; ++i) {
        srcs[i]->allocator = aws_default_allocator();
        srcs[i]->capacity = nondet_size_t();
        __CPROVER_assume(srcs[i]->capacity <= MAX_BUFFER_SIZE);
        srcs[i]->len = nondet_size_t();
        __CPROVER_assume(srcs[i]->len <= srcs[i]->capacity);
        if (srcs[i]->capacity > 0) {
            srcs[i]->buffer = malloc(srcs[i]->capacity);
            __CPROVER_assume(srcs[i]->buffer != NULL);
        } else {
            srcs[i]->buffer = NULL;
        }
    }

    /* structural validity assumptions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer1, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer1));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer2, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer2));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer3, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer3));

    /* snapshot of pre‑state */
    struct aws_byte_buf old_dest = dest;
    uint8_t *old_dest_buf = NULL;
    if (old_dest.capacity > 0) {
        old_dest_buf = malloc(old_dest.capacity);
        __CPROVER_assume(old_dest_buf != NULL);
        memcpy(old_dest_buf, old_dest.buffer, old_dest.capacity);
    }

    struct aws_byte_buf old_buf1 = buffer1;
    uint8_t *old_buf1_data = NULL;
    if (old_buf1.capacity > 0) {
        old_buf1_data = malloc(old_buf1.capacity);
        __CPROVER_assume(old_buf1_data != NULL);
        memcpy(old_buf1_data, old_buf1.buffer, old_buf1.capacity);
    }

    struct aws_byte_buf old_buf2 = buffer2;
    uint8_t *old_buf2_data = NULL;
    if (old_buf2.capacity > 0) {
        old_buf2_data = malloc(old_buf2.capacity);
        __CPROVER_assume(old_buf2_data != NULL);
        memcpy(old_buf2_data, old_buf2.buffer, old_buf2.capacity);
    }

    struct aws_byte_buf old_buf3 = buffer3;
    uint8_t *old_buf3_data = NULL;
    if (old_buf3.capacity > 0) {
        old_buf3_data = malloc(old_buf3.capacity);
        __CPROVER_assume(old_buf3_data != NULL);
        memcpy(old_buf3_data, old_buf3.buffer, old_buf3.capacity);
    }

    /* call the function under verification */
    int ret = aws_byte_buf_cat(&dest, 3, &buffer1, &buffer2, &buffer3);

    /* 1. Return value must be a valid status */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* 2. Output buffer invariants */
    if (ret == AWS_OP_SUCCESS) {
        size_t total_appended = buffer1.len + buffer2.len + buffer3.len;
        assert(dest.len == old_dest.len + total_appended);
        assert(dest.len <= dest.capacity);
        /* buffer pointer must stay the same (no reallocation) */
        assert(dest.buffer == old_dest.buffer);
        /* verify concatenated contents */
        size_t off = old_dest.len;
        if (buffer1.len > 0) {
            assert(memcmp(dest.buffer + off, buffer1.buffer, buffer1.len) == 0);
            off += buffer1.len;
        }
        if (buffer2.len > 0) {
            assert(memcmp(dest.buffer + off, buffer2.buffer, buffer2.len) == 0);
            off += buffer2.len;
        }
        if (buffer3.len > 0) {
            assert(memcmp(dest.buffer + off, buffer3.buffer, buffer3.len) == 0);
        }
        /* unchanged region before original length */
        if (old_dest.len > 0) {
            assert(memcmp(dest.buffer, old_dest_buf, old_dest.len) == 0);
        }
    } else {
        /* on error, dest must be unchanged */
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.buffer == old_dest.buffer);
        if (old_dest.capacity > 0) {
            assert(memcmp(dest.buffer, old_dest_buf, old_dest.capacity) == 0);
        }
    }

    /* 3. Frame conditions for source buffers (must remain unchanged) */
    assert(buffer1.len == old_buf1.len);
    assert(buffer1.capacity == old_buf1.capacity);
    assert(buffer1.buffer == old_buf1.buffer);
    if (buffer1.capacity > 0) {
        assert(memcmp(buffer1.buffer, old_buf1_data, buffer1.capacity) == 0);
    }

    assert(buffer2.len == old_buf2.len);
    assert(buffer2.capacity == old_buf2.capacity);
    assert(buffer2.buffer == old_buf2.buffer);
    if (buffer2.capacity > 0) {
        assert(memcmp(buffer2.buffer, old_buf2_data, buffer2.capacity) == 0);
    }

    assert(buffer3.len == old_buf3.len);
    assert(buffer3.capacity == old_buf3.capacity);
    assert(buffer3.buffer == old_buf3.buffer);
    if (buffer3.capacity > 0) {
        assert(memcmp(buffer3.buffer, old_buf3_data, buffer3.capacity) == 0);
    }

    /* clean up */
    free(old_dest_buf);
    free(old_buf1_data);
    free(old_buf2_data);
    free(old_buf3_data);
    free(dest.buffer);
    free(buffer1.buffer);
    free(buffer2.buffer);
    free(buffer3.buffer);

    return 0;
}
