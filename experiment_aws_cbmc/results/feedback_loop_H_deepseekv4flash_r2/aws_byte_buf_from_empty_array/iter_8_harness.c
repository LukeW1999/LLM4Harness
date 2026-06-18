#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_buf_from_empty_array_harness() {
    /* Non-deterministic array pointer and capacity */
    uint8_t *array = nondet_uint8_t_ptr();
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(array != NULL);  /* Function expects non-null array */

    /* Call the function */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, capacity);

    /* Postconditions */
    assert(buf.buffer == array);
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);
}
