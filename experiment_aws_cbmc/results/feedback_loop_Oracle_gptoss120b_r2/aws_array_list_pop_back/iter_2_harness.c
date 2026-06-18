#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEM_SIZE   64
#define MAX_CAPACITY    10

void aws_array_list_pop_back_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic item size (>0) */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* nondeterministic capacity (number of items that can fit) */
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_CAPACITY);

    /* compute current_size = capacity * item_size (no overflow) */
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(capacity, item_size, &current_size));

    /* allocate backing storage */
    uint8_t *data = NULL;
    if (current_size > 0) {
        data = (uint8_t *)aws_mem_acquire(allocator, current_size);
        __CPROVER_assume(data != NULL);
        /* initialize with nondeterministic bytes */
        for (size_t i = 0; i < current_size; ++i) {
            data[i] = (uint8_t)__CPROVER_nondet_uint();
        }
    }

    /* nondeterministic length (number of valid items) */
    size_t length;
    __CPROVER_assume(length <= capacity);

    /* initialize list */
    list.alloc = allocator;
    list.data = data;
    list.item_size = item_size;
    list.current_size = current_size;
    list.length = length;

    /* Ensure the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Snapshot of relevant state */
    size_t old_length = list.length;
    uint8_t *old_data = NULL;
    if (current_size > 0) {
        old_data = (uint8_t *)malloc(current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, current_size);
    }

    /* Call the function under test */
    int ret = aws_array_list_pop_back(&list);

    /* Post‑condition checks */
    if (old_length > 0) {
        /* Success case */
        assert(ret == AWS_OP_SUCCESS);
        assert(list.length == old_length - 1);
        /* The popped element must be zeroed */
        size_t popped_offset = item_size * (old_length - 1);
        for (size_t i = 0; i < item_size; ++i) {
            assert(((uint8_t *)list.data)[popped_offset + i] == 0);
        }
        /* All preceding bytes must be unchanged */
        for (size_t i = 0; i < popped_offset; ++i) {
            assert(((uint8_t *)list.data)[i] == old_data[i]);
        }
        /* All trailing bytes (if any) must be unchanged */
        size_t trailing_start = popped_offset + item_size;
        for (size_t i = trailing_start; i < current_size; ++i) {
            assert(((uint8_t *)list.data)[i] == old_data[i]);
        }
    } else {
        /* Failure case: list was empty */
        assert(ret != AWS_OP_SUCCESS);
        assert(list.length == 0);
        /* No modification to data buffer */
        if (current_size > 0) {
            for (size_t i = 0; i < current_size; ++i) {
                assert(((uint8_t *)list.data)[i] == old_data[i]);
            }
        }
    }

    /* Frame conditions: fields other than length and data contents must be unchanged */
    assert(list.alloc == allocator);
    assert(list.item_size == item_size);
    assert(list.current_size == current_size);
    assert(list.data == data);

    /* Clean up */
    if (data) {
        aws_mem_release(allocator, data);
    }
    if (old_data) {
        free(old_data);
    }

    return 0;
}
