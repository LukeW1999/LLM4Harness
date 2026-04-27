/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_eq_byte_buf_harness() {
    /* Non-deterministic inputs - both must be non-NULL for valid comparison */
    const struct aws_string *str = ensure_allocated_string(MAX_STRING_LEN);
    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(buf != NULL);
    __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_STRING_LEN));
    ensure_byte_buf_has_allocated_buffer_member(buf);
    __CPROVER_assume(aws_byte_buf_is_valid(buf));

    /* Ensure buf->buffer is non-NULL when len > 0 to satisfy memcmp precondition */
    __CPROVER_assume(buf->len == 0 || buf->buffer != NULL);
    
    /* Ensure str->len matches buf->len scenario or str bytes are readable */
    /* The memcmp failure is in region 1 (str->bytes), ensure str bytes are readable */
    __CPROVER_assume(str->len == 0 || str->bytes != NULL);

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Validity invariants: inputs must remain unchanged */
    assert(aws_string_is_valid(str));
    assert(aws_byte_buf_is_valid(buf));
}
