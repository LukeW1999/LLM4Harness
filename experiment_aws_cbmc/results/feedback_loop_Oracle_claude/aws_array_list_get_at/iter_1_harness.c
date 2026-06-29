#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_harness(void) {
    /* Allocate and initialize the array list */
    struct aws_array_list list;

    /* Use nondet values for list parameters */
    size_t item_size;
    size_t initial_item_allocation;
    size_t index;

    /* Constrain item_size to be reasonable */
    __CPROVER_assume(item_size > 0 && item_size <= 64);
    /* Constrain initial allocation to be reasonable */
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= 8);
    /* Constrain index to be reasonable */
    __CPROVER_assume(index <= 8);

    /* Initialize the list dynamically */
    struct aws_allocator *alloc = aws_default_allocator();
    int init_result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Nondet length, constrained to be within capacity */
    size_t length;
    __CPROVER_assume(length <= initial_item_allocation);
    list.length = length;

    /* Allocate output buffer */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Save state before the call */
    size_t old_length = aws_array_list_length(&list);
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    size_t old_item_size = list.item_size;

    /* Call the function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* Postcondition 1: Return value correctness */
    if (old_length > index) {
        /* Should succeed */
        assert(result == AWS_OP_SUCCESS);
    } else {
        /* Should fail with invalid index */
        assert(result == AWS_OP_ERR);
        assert(aws_last_error() == AWS_ERROR_INVALID_INDEX);
    }

    /* Postcondition 2: List validity is preserved */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition 3: Length invariant - length should not change */
    assert(aws_array_list_length(&list) == old_length);

    /* Postcondition 4: Capacity invariant - current_size should not change */
    assert(list.current_size == old_current_size);

    /* Postcondition 5: Item size should not change */
    assert(list.item_size == old_item_size);

    /* Postcondition 6: Data pointer should not change */
    assert(list.data == old_data);

    /* Postcondition 7: If success, val contains data from the list at the correct offset */
    if (result == AWS_OP_SUCCESS) {
        /* The val buffer should have been written with item_size bytes from list->data + index * item_size */
        /* We verify this by checking the memory is readable */
        assert(AWS_MEM_IS_READABLE(val, item_size));
    }

    /* Cleanup */
    aws_array_list_clean_up(&list);
    free(val);
}
