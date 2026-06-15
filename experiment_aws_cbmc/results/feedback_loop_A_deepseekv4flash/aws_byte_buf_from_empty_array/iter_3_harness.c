#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness() {
    /* Parameters */
    size_t capacity;
    uint8_t *array;

    /* Non-deterministic input */
    capacity = nondet_size_t();
    /* Bound capacity to avoid too large allocation */
    __CPROVER_assume(capacity <= 10);

    /* Allow array to be NULL or non-null */
    if (nondet_bool()) {
        array = malloc(capacity); /* can return NULL if capacity == 0, but that's fine */
    } else {
        array = NULL;
    }

    /* Call the function */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, capacity);

    /* Postconditions */
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.buffer == array);
    assert(buf.allocator == NULL);

    /* If buffer is non-null, it should be writable */
    if (buf.buffer != NULL) {
        assert(AWS_MEM_IS_WRITABLE(buf.buffer, buf.capacity));
    }

    /* Clean up */
    free(array);
}
