#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/assert.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_buf_append_harness(void) {
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* allocator */
    to.allocator = aws_default_allocator();

    /* nondet length and capacity for the destination buffer */
    to.len = nondet_size_t();
    to.capacity = nondet_size_t();
    __CPROVER_assume(to.capacity >= to.len);

    /* allocate destination buffer if capacity > 0 */
    if (to.capacity > 0) {
        to.buffer = malloc(to.capacity);
        __CPROVER_assume(to.buffer != NULL);
    } else {
        to.buffer = NULL;
    }

    /* nondet length for the source cursor */
    from.len = nondet_size_t();

    /* allocate source pointer if length > 0 */
    if (from.len > 0) {
        from.ptr = malloc(from.len);
        __CPROVER_assume(from.ptr != NULL);
    } else {
        from.ptr = NULL;
    }

    /* assume the inputs satisfy the validity predicates */
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* snapshot pre‑call state */
    size_t old_to_len = to.len;
    size_t old_to_capacity = to.capacity;
    uint8_t *old_to_buffer = to.buffer;
    uint8_t *old_to_contents = NULL;
    if (to.buffer != NULL && to.len > 0) {
        old_to_contents = malloc(to.len);
        __CPROVER_assume(old_to_contents != NULL);
        memcpy(old_to_contents, to.buffer, to.len);
    }

    size_t old_from_len = from.len;
    const uint8_t *old_from_ptr = from.ptr;
    uint8_t *old_from_contents = NULL;
    if (from.ptr != NULL && from.len > 0) {
        old_from_contents = malloc(from.len);
        __CPROVER_assume(old_from_contents != NULL);
        memcpy(old_from_contents, from.ptr, from.len);
    }

    /* call the function under verification */
    int result = aws_byte_buf_append(&to, &from);

    /* ASSERT_POSTCONDITIONS_HERE */
}
