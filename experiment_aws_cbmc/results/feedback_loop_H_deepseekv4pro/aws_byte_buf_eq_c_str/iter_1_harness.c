#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* c_str is a null-terminated string */
    size_t c_str_len;
    const char *c_str = nondet_bool() ? NULL : malloc(c_str_len + 1);
    if (c_str != NULL) {
        __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
        c_str[c_str_len] = '\0';
    }

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old = buf;

    /* 3. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 4. Assert postconditions */
    /* The function is a pure comparison — it does not modify buf */
    /* buf must be completely unchanged */
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(buf.allocator == old.allocator);

    /* 5. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));

    /* 6. Free allocated memory */
    if (c_str != NULL) {
        free((void *)c_str);
    }
}
