/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_string_harness() {
    /* Non-deterministically decide whether src is NULL */
    const struct aws_string *src;

    if (nondet_bool()) {
        /* NULL case */
        src = NULL;
    } else {
        /* Non-NULL case: allocate a valid aws_string */
        struct aws_string *str = ensure_allocated_string_with_bounded_len(MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(str));
        src = str;
    }

    /* Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* Postconditions */
    if (src == NULL) {
        /* If src is NULL, returns an empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If src is non-NULL, returns cursor pointing to string bytes with matching length */
        assert(result.len == src->len);
        assert(result.ptr == aws_string_bytes(src));
        if (result.len > 0) {
            assert(result.ptr != NULL);
            assert_bytes_match(result.ptr, aws_string_bytes(src), result.len);
        }
    }

    /* Validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
```

Wait, I need to check if `ensure_allocated_string_with_bounded_len` is available. Looking at the proof helpers, I don't see that function listed. Let me use what's available.

```c
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_string_harness() {
    /* Non-deterministically decide whether src is NULL */
    bool is_null = nondet_bool();

    const struct aws_string *src;

    if (is_null) {
        src = NULL;
    } else {
        /* Allocate a valid aws_string with bounded length */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the string header + data bytes + null terminator */
        struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);
        /* Initialize the length field (bytes field is non-deterministic) */
        __CPROVER_assume(aws_string_is_valid(str));
        /* Force the len field to match our bounded len */
        __CPROVER_assume(str->len == len);
        src = str;
    }

    /* Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* Postconditions */
    if (src == NULL) {
        /* If src is NULL, returns an empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If src is non-NULL, returns cursor pointing to string bytes with matching length */
        assert(result.len == src->len);
        assert(result.ptr == aws_string_bytes(src));
        if (result.len > 0) {
            assert(result.ptr != NULL);
        }
    }

    /* Validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
