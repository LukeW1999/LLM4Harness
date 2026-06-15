#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string *str;
    struct aws_byte_buf *buf;

    /* nondet decide whether string is NULL */
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate storage for struct + flexible array on the stack */
        uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        str = (struct aws_string *)storage;
        str->allocator = aws_default_allocator();
        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
    } else {
        str = NULL;
    }

    /* nondet decide whether buffer is NULL */
    if (nondet_bool()) {
        uint8_t buffer_storage[MAX_BUFFER_SIZE];
        struct aws_byte_buf local_buf;

        local_buf.allocator = aws_default_allocator();

        size_t capacity = nondet_size_t();
        __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
        local_buf.capacity = capacity;

        size_t len = nondet_size_t();
        __CPROVER_assume(len <= capacity);
        local_buf.len = len;

        local_buf.buffer = buffer_storage;

        for (size_t i = 0; i < capacity; ++i) {
            local_buf.buffer[i] = nondet_uint8_t();
        }

        buf = &local_buf;
    } else {
        buf = NULL;
    }

    /* Save old state for immutability checks */
    struct aws_string old_str;
    if (str != NULL) {
        old_str = *str;
    }

    struct aws_byte_buf old_buf;
    if (buf != NULL) {
        old_buf = *buf;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Post‑condition checks */
    if (str == NULL || buf == NULL) {
        assert(!result);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, buf->buffer, buf->len);
        assert(result == expected);
    }

    /* Immutability checks */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(memcmp(str->bytes, old_str.bytes, str->len) == 0);
    }

    if (buf != NULL) {
        assert(buf->allocator == old_buf.allocator);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->len == old_buf.len);
        assert(buf->buffer == old_buf.buffer);
        assert(memcmp(buf->buffer, old_buf.buffer, buf->capacity) == 0);
    }

    /* Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
