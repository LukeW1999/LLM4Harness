#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_from_empty_array_harness() {
    /* Parameters */
    uint8_t *array;
    size_t capacity;

    /* Nondeterministic choices */
    bool array_is_null = nondet_bool();
    capacity = nondet_size_t();

    /* Assumptions to ensure valid inputs */
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    if (array_is_null) {
        __CPROVER_assume(capacity == 0);
        array = NULL;
    } else {
        array = (uint8_t *)malloc(capacity);
        __CPROVER_assume(array != NULL);
        /* Fill array with nondet bytes */
        if (capacity > 0) {
            for (size_t i = 0; i < capacity; i++) {
                array[i] = nondet_uint8_t();
            }
        }
    }

    /* Save old state of array (if non-NULL) */
    struct store_byte_from_buffer array_store;
    if (array != NULL && capacity > 0) {
        save_byte_from_array(array, capacity, &array_store);
    }

    /* Call function */
    struct aws_byte_buf result = aws_byte_buf_from_empty_array(array, capacity);

    /* Postconditions */
    assert(result.allocator == NULL);
    assert(result.buffer == array);
    assert(result.len == 0);
    assert(result.capacity == capacity);
    assert(aws_byte_buf_is_valid(&result));

    /* Array must remain unchanged */
    if (array != NULL && capacity > 0) {
        assert_byte_from_buffer_matches(array, &array_store);
    }
}
