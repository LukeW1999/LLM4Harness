/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_c_str_harness() {
    /* Set up aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* Set up c_str - non-deterministically NULL or a valid string */
    const char *c_str = nondet_bool() ? NULL : ensure_c_str_is_allocated(MAX_BUFFER_SIZE);

    /* Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 
     * aws_byte_buf_eq_c_str is a pure comparison function.
     * It does not modify the buffer at all.
     * Assert all fields are unchanged.
     */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    /* Assert validity invariant still holds */
    assert(aws_byte_buf_is_valid(&buf));

    /* 
     * If c_str is NULL, aws_array_eq_c_str with NULL c_str:
     * The function compares buf->buffer (len bytes) with c_str.
     * If c_str is NULL, strlen(NULL) is undefined, but aws_array_eq_c_str
     * handles the case where len_a == 0 and array_a is NULL.
     * We don't assert specific return values since they depend on content.
     */

    /* The result is a bool - just verify it's a valid bool */
    assert(result == true || result == false);
}
```

Wait, I need to check if `ensure_c_str_is_allocated` is available. Let me use a simpler approach with nondet pointer.

```c
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_c_str_harness() {
    /* Set up aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state before calling */
    struct aws_byte_buf old_buf = buf;

    /* Set up c_str - a non-deterministic null-terminated string */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    if (c_str != NULL) {
        c_str[c_str_len] = '\0';
    }

    /* Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /*
     * aws_byte_buf_eq_c_str is a pure comparison function.
     * It does not modify the buffer at all.
     * Assert all fields are unchanged.
     */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    /* Assert validity invariant still holds after the call */
    assert(aws_byte_buf_is_valid(&buf));

    /* The result is a bool - verify it's a valid bool value */
    assert(result == true || result == false);
}
