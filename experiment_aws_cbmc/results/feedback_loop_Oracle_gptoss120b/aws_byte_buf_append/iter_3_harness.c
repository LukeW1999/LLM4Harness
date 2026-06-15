#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

extern size_t nondet_size_t(void);

int aws_byte_buf_append_harness(void) {
    const size_t MAX_BUFFER_SIZE = 256;

    struct aws_byte_buf to;
    struct aws_byte_cursor from;
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondet allocate and initialize `to` */
    to.capacity = nondet_size_t();
    __CPROVER_assume(to.capacity <= MAX_BUFFER_SIZE);
    if (to.capacity > 0) {
        to.buffer = malloc(to.capacity);
        __CPROVER_assume(to.buffer != NULL);
    } else {
        to.buffer = NULL;
    }
    to.len = nondet_size_t();
    __CPROVER_assume(to.len <= to.capacity);
    to.allocator = allocator;

    /* nondet allocate and initialize `from` */
    from.len = nondet_size_t();
    __CPROVER_assume(from.len <= MAX_BUFFER_SIZE);
    if (from.len > 0) {
        from.ptr = malloc(from.len);
        __CPROVER_assume(from.ptr != NULL);
    } else {
        from.ptr = NULL;
    }

    /* structural validity assumptions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* save old state for post‑condition checks */
    size_t old_to_len      = to.len;
    size_t old_to_capacity = to.capacity;
    uint8_t *old_to_buffer = to.buffer;
    size_t old_from_len    = from.len;
    const uint8_t *old_from_ptr = from.ptr;

    uint8_t *old_buf_copy = NULL;
    if (old_to_capacity > 0) {
        old_buf_copy = malloc(old_to_capacity);
        __CPROVER_assume(old_buf_copy != NULL);
        memcpy(old_buf_copy, old_to_buffer, old_to_capacity);
    }

    int ret = aws_byte_buf_append(&to, &from);

    /* 1. Return‑value / error‑code correctness */
    if (ret == AWS_OP_SUCCESS) {
        /* enough space was available */
        assert(to.capacity - old_to_len >= old_from_len);
        /* length updated correctly */
        assert(to.len == old_to_len + old_from_len);
    } else {
        /* not enough space */
        assert(to.capacity - old_to_len < old_from_len);
        /* length unchanged */
        assert(to.len == old_to_len);
    }

    /* 2. Output‑buffer invariants */
    assert(to.capacity == old_to_capacity);
    assert(to.allocator == allocator);

    /* 3. Frame conditions – memory outside the written region unchanged */
    if (old_to_capacity > 0) {
        if (old_to_len > 0) {
            assert(memcmp(to.buffer, old_buf_copy, old_to_len) == 0);
        }
        size_t new_len = to.len;
        if (new_len < old_to_capacity) {
            assert(memcmp(to.buffer + new_len,
                         old_buf_copy + new_len,
                         old_to_capacity - new_len) == 0);
        }
    }

    /* `from` must remain unchanged */
    assert(from.len == old_from_len);
    assert(from.ptr == old_from_ptr);
    if (old_from_len > 0) {
        assert(memcmp(from.ptr, old_from_ptr, old_from_len) == 0);
    }

    /* clean up */
    free(to.buffer);
    free(from.ptr);
    free(old_buf_copy);
    return 0;
}
