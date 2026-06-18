#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_eq_byte_buf_harness(void) {
    /* nondeterministic choice for str */
    struct aws_string *str = NULL;
    if (!__CPROVER_nondet_bool()) {
        /* allocate a string with a nondeterministic length up to MAX_STRING_LEN */
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        /* flexible array member: allocate sizeof(struct aws_string) + len - 1 */
        size_t alloc_size = sizeof(struct aws_string) + len - 1;
        str = malloc(alloc_size);
        __CPROVER_assume(str != NULL);
        str->allocator = NULL;               /* allocator is irrelevant for the proof */
        str->len = len;
        /* nondeterministically fill the bytes */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = __CPROVER_nondet_uint8_t();
        }
    }

    /* nondeterministic choice for buf pointer */
    struct aws_byte_buf *buf_ptr = NULL;
    struct aws_byte_buf buf;
    if (!__CPROVER_nondet_bool()) {
        /* allocate a buffer with a nondeterministic capacity up to MAX_STRING_LEN */
        size_t capacity = __CPROVER_nondet_size_t();
        __CPROVER_assume(capacity <= MAX_STRING_LEN);
        uint8_t *buffer_mem = malloc(capacity);
        __CPROVER_assume(buffer_mem != NULL);
        buf.buffer = buffer_mem;
        buf.capacity = capacity;
        buf.len = __CPROVER_nondet_size_t();
        __CPROVER_assume(buf.len <= capacity);
        /* nondeterministically fill the buffer contents */
        for (size_t i = 0; i < buf.len; ++i) {
            buf.buffer[i] = __CPROVER_nondet_uint8_t();
        }
        buf_ptr = &buf;
    }

    /* structural validity assumptions */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    if (buf_ptr != NULL) {
        __CPROVER_assume(aws_byte_buf_is_bounded(buf_ptr, MAX_STRING_LEN));
        __CPROVER_assume(aws_byte_buf_is_valid(buf_ptr));
    }

    /* snapshot memory for frame condition checks */
    struct aws_string *old_str = NULL;
    if (str != NULL) {
        size_t snap_size = sizeof(struct aws_string) + str->len - 1;
        old_str = malloc(snap_size);
        __CPROVER_assume(old_str != NULL);
        memcpy(old_str, str, snap_size);
    }
    struct aws_byte_buf old_buf = {0};
    if (buf_ptr != NULL) {
        old_buf = *buf_ptr;
        if (buf_ptr->buffer != NULL && buf_ptr->len > 0) {
            uint8_t *old_buf_mem = malloc(buf_ptr->len);
            __CPROVER_assume(old_buf_mem != NULL);
            memcpy(old_buf_mem, buf_ptr->buffer, buf_ptr->len);
            old_buf.buffer = old_buf_mem;
        }
    }

    /* call the function under verification */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* postcondition 1: return value correctness */
    bool expected;
    if (str == NULL && buf_ptr == NULL) {
        expected = true;
    } else if (str == NULL || buf_ptr == NULL) {
        expected = false;
    } else {
        expected = aws_array_eq(str->bytes, str->len, buf_ptr->buffer, buf_ptr->len);
    }
    assert(result == expected);

    /* postcondition 2: no modification of inputs (frame condition) */
    if (str != NULL) {
        size_t snap_size = sizeof(struct aws_string) + str->len - 1;
        assert(memcmp(str, old_str, snap_size) == 0);
    }
    if (buf_ptr != NULL) {
        assert(buf_ptr->len == old_buf.len);
        assert(buf_ptr->capacity == old_buf.capacity);
        if (buf_ptr->len > 0) {
            assert(memcmp(buf_ptr->buffer, old_buf.buffer, buf_ptr->len) == 0);
        }
    }

    return 0;
}
