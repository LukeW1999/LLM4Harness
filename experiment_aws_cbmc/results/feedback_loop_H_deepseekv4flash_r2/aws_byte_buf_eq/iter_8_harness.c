#include <aws/common/byte_buf.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_byte_buf_eq_harness() {
    /* parameters */
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    /* preconditions: both buffers are bounded and valid */
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* call the function */
    bool result = aws_byte_buf_eq(&a, &b);

    /* postconditions based on the specification */
    if (a.len != b.len) {
        /* lengths differ -> must be false */
        assert(!result);
    } else {
        /* lengths equal -> result depends on content */
        if (a.len == 0) {
            /* both empty -> equal */
            assert(result);
        } else {
            /* content must match exactly */
            assert(result == (memcmp(a.buffer, b.buffer, a.len) == 0));
        }
    }

    /* both buffers should remain valid */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
