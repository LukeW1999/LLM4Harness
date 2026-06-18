#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Maximum size for the nondeterministic cursor */
#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic source cursor */
    struct aws_byte_cursor src;
    size_t nondet_len = nondet_size_t();
    __CPROVER_assume(nondet_len <= MAX_BUFFER_SIZE);
    src.len = nondet_len;

    if (src.len > 0) {
        src.ptr = malloc(src.len);
        __CPROVER_assume(src.ptr != NULL);
        /* initialize source memory with nondeterministic values */
        for (size_t i = 0; i < src.len; ++i) {
            src.ptr[i] = nondet_uint8_t();
        }
    } else {
        src.ptr = NULL;
    }

    /* structural validity assumptions */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* keep a copy of the source for later frame condition checks */
    struct aws_byte_cursor src_before = src;
    uint8_t *src_before_buf = NULL;
    if (src_before.len > 0) {
        src_before_buf = malloc(src_before.len);
        __CPROVER_assume(src_before_buf != NULL);
        memcpy(src_before_buf, src_before.ptr, src_before.len);
    }

    int ret = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Post‑condition checks */
    if (ret == AWS_OP_SUCCESS) {
        /* length / capacity invariants */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);

        if (src.len == 0) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            /* content must be copied correctly */
            assert(!memcmp(dest.buffer, src.ptr, src.len));
        }
    } else {
        /* In the error case the function should not have allocated any buffer */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == allocator);
    }

    /* Frame condition: source cursor must remain unchanged */
    assert(src.ptr == src_before.ptr);
    assert(src.len == src_before.len);
    if (src_before.len > 0) {
        assert(!memcmp(src.ptr, src_before_buf, src_before.len));
    }

    /* Clean up allocated memory */
    if (src.ptr) {
        free(src.ptr);
    }
    if (src_before_buf) {
        free(src_before_buf);
    }
    if (dest.buffer) {
        free(dest.buffer);
    }

    return 0;
}
