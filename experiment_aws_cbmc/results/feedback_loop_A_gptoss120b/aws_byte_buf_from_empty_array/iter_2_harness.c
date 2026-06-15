/* CBMC harness for aws_byte_buf_from_empty_array() */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf;
    uint8_t *array = nondet_pointer();
    size_t capacity = nondet_uint64();

    /* Bound the capacity to avoid overflow */
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* If the array pointer is non‑NULL, allocate a buffer of the given capacity */
    if (array != NULL) {
        __CPROVER_assume(capacity > 0);
        array = malloc(capacity);
        __CPROVER_assume(array != NULL);
    } else {
        __CPROVER_assume(capacity == 0);
    }

    /* Ensure the destination byte buffer structure is bounded (its fields may be uninitialized) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Save the old state of the destination buffer for immutability checks */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    aws_byte_buf_from_empty_array(&buf, array, capacity);

    /* Post‑condition checks */
    assert(buf.buffer == array);
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));

    /* Fields that are not documented as changing must retain their original values.
       In aws_byte_buf, all fields are set by the function, so no additional checks are needed. */

    /* Global validity invariant for the source array (if any) */
    if (array != NULL) {
        /* The array memory should remain allocated; no further checks required. */
    }
}
