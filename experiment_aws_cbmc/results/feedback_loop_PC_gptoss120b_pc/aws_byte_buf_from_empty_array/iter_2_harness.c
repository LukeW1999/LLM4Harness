#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_from_empty_array_harness(void) {
    /* nondeterministically choose a capacity within the global bound */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* allocate an array of the chosen capacity (may be NULL if capacity is 0) */
    uint8_t *array = NULL;
    if (capacity > 0) {
        array = (uint8_t *)malloc(capacity);
        __CPROVER_assume(array != NULL);
    }

    /* destination byte buffer (output) – uninitialized before the call */
    struct aws_byte_buf buf;

    /* Call the function under test */
    aws_byte_buf_from_empty_array(&buf, array, capacity);

    /* Post‑condition checks */
    assert(buf.buffer == array);
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));

    /* If an array was provided, its contents must remain unchanged */
    if (array != NULL && capacity > 0) {
        /* No bytes should have been written; the memory is untouched */
        /* (nothing to check because len is 0) */
    }

    /* Global invariants */
    assert(aws_byte_buf_is_valid(&buf));
}
