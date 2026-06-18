#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_eq_c_str_harness() {
    /* Initialize a valid byte buffer. */
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();
    buf.allocator = allocator;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Prepare a possibly‑NULL C string. */
    size_t c_str_len;
    bool is_null;
    const char *c_str;
    if (is_null) {
        c_str = NULL;
    } else {
        __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
        c_str = (const char *)malloc(c_str_len + 1);
        if (c_str != NULL) {
            c_str[c_str_len] = '\0';
        }
    }

    /* Save the state before the call. */
    struct aws_byte_buf old = buf;

    /* Function under verification. */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* Check that the buffer has not been modified. */
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(buf.allocator == old.allocator);
    assert(aws_byte_buf_is_valid(&buf));

    /* Clean up the dynamically allocated C string. */
    if (!is_null && c_str != NULL) {
        free((void *)c_str);
    }
}
