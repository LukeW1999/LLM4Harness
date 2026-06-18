#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Nondeterministic item size, must be > 0 */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    list.item_size = item_size;

    /* Nondeterministic capacity (number of items that can be stored) */
    size_t capacity;
    __CPROVER_assume(capacity <= 1024); /* bound for tractability */
    size_t current_size;
    __CPROVER_assume(aws_mul_size_checked(capacity, item_size, &current_size) == AWS_OP_SUCCESS);
    list.current_size = current_size;
    list.alloc = allocator;

    /* Allocate backing buffer */
    uint8_t *buffer = NULL;
    if (current_size > 0) {
        buffer = (uint8_t *)aws_mem_acquire(allocator, current_size);
        __CPROVER_assume(buffer != NULL);
        /* Fill with nondeterministic data */
        for (size_t i = 0; i < current_size; ++i) {
            buffer[i] = (uint8_t)__CPROVER_nondet_uint();
        }
    }
    list.data = buffer;

    /* Nondeterministic length, must be <= capacity */
    size_t length;
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /* Ensure the list is initially valid */
    assert(aws_array_list_is_valid(&list));

    /* Snapshot of the buffer for frame condition checking */
    uint8_t *old_buffer = NULL;
    if (current_size > 0) {
        old_buffer = (uint8_t *)malloc(current_size);
        __CPROVER_assume(old_buffer != NULL);
        memcpy(old_buffer, buffer, current_size);
    }

    size_t old_length = length;

    int ret = aws_array_list_pop_back(&list);

    /* Postcondition 1: return value correctness */
    if (old_length > 0) {
        assert(ret == AWS_OP_SUCCESS);
    } else {
        assert(ret == aws_raise_error(AWS_ERROR_LIST_EMPTY));
    }

    /* Postcondition 2: length invariants */
    if (old_length > 0) {
        assert(list.length == old_length - 1);
    } else {
        assert(list.length == old_length);
    }

    /* Postcondition 3: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Frame condition: memory not modified beyond contract */
    if (current_size > 0) {
        size_t bytes_to_check = list.length * item_size;
        /* Bytes belonging to remaining elements must be unchanged */
        for (size_t i = 0; i < bytes_to_check; ++i) {
            assert(buffer[i] == old_buffer[i]);
        }
        if (old_length > 0) {
            /* The popped element region must be zeroed */
            size_t popped_offset = old_length - 1;
            size_t popped_start = popped_offset * item_size;
            for (size_t i = popped_start; i < popped_start + item_size; ++i) {
                assert(buffer[i] == 0);
            }
        } else {
            /* No pop occurred, entire buffer unchanged */
            for (size_t i = 0; i < current_size; ++i) {
                assert(buffer[i] == old_buffer[i]);
            }
        }
    }

    /* Clean up */
    if (buffer) {
        aws_mem_release(allocator, buffer);
    }
    if (old_buffer) {
        free(old_buffer);
    }

    return;
}
