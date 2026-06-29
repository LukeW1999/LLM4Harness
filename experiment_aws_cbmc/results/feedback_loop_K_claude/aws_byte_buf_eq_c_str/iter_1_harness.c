#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound on buffer/string sizes for CBMC tractability */
#ifndef MAX_BUF_SIZE
#define MAX_BUF_SIZE 8
#endif

void aws_byte_buf_eq_c_str_harness(void) {
    /* Allocate and initialize a non-deterministic aws_byte_buf */
    struct aws_byte_buf buf;

    /* Non-deterministic length and capacity */
    size_t len;
    size_t capacity;
    __CPROVER_assume(len <= MAX_BUF_SIZE);
    __CPROVER_assume(capacity >= len);
    __CPROVER_assume(capacity <= MAX_BUF_SIZE);

    buf.len = len;
    buf.capacity = capacity;
    buf.allocator = aws_default_allocator();

    /* Set up buffer memory */
    if (capacity > 0) {
        buf.buffer = malloc(capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

    /* Precondition: buf must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Allocate a non-deterministic null-terminated c_str of bounded length */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUF_SIZE);

    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Ensure c_str is null-terminated */
    c_str[c_str_len] = '\0';

    /* Save state before call for frame condition checks */
    size_t old_len = buf.len;
    size_t old_capacity = buf.capacity;
    uint8_t *old_buffer = buf.buffer;

    /* Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* Postcondition: frame conditions - buf is not modified */
    assert(buf.len == old_len);
    assert(buf.capacity == old_capacity);
    assert(buf.buffer == old_buffer);

    /* Postcondition: result is a valid bool */
    assert(result == true || result == false);

    /* Postcondition: if result is true, then lengths must match */
    if (result) {
        /* buf->len must equal strlen(c_str) */
        size_t computed_c_str_len = 0;
        for (size_t i = 0; i <= MAX_BUF_SIZE; i++) {
            if (c_str[i] == '\0') {
                computed_c_str_len = i;
                break;
            }
        }
        assert(buf.len == computed_c_str_len);

        /* All bytes must match */
        for (size_t i = 0; i < buf.len; i++) {
            assert(buf.buffer[i] == (uint8_t)c_str[i]);
        }
    }

    /* Postcondition: if lengths differ, result must be false */
    if (buf.len != c_str_len) {
        /* c_str_len is the actual length we set, so if they differ, result should be false */
        /* Note: c_str_len is the length before null terminator */
        assert(!result);
    }

    /* Postcondition: if buf contains a null byte within its length, result must be false */
    if (buf.len > 0 && buf.buffer != NULL) {
        for (size_t i = 0; i < buf.len; i++) {
            if (buf.buffer[i] == '\0') {
                assert(!result);
                break;
            }
        }
    }
}
