#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_clear_harness(void) {
    struct aws_array_list list;

    /* Non-deterministically choose between a zeroed list and a valid list */
    if (nondet_bool()) {
        /* Zeroed list case */
        AWS_ZERO_STRUCT(list);
    } else {
        /* Valid list case - initialize with dynamic allocation */
        size_t item_size;
        __CPROVER_assume(item_size > 0 && item_size <= 128);

        size_t initial_item_allocation;
        __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= 16);

        struct aws_allocator *allocator = aws_default_allocator();

        int init_result = aws_array_list_init_dynamic(
            &list,
            allocator,
            initial_item_allocation,
            item_size);

        /* Only proceed if initialization succeeded */
        __CPROVER_assume(init_result == AWS_OP_SUCCESS);

        /* Non-deterministically set the length to something valid */
        size_t length;
        __CPROVER_assume(length <= initial_item_allocation);
        list.length = length;
    }

    /* Save state before the call */
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Precondition: list must be zeroed or valid */
    assert(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    /* Call the function under test */
    aws_array_list_clear(&list);

    /* Postcondition 1: The list must still be zeroed or valid after the call */
    assert(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    /* Postcondition 2: Length invariant - length must be 0 after clear */
    assert(list.length == 0);

    /* Postcondition 3: Frame conditions - current_size, data pointer, item_size, and alloc must not change */
    assert(list.current_size == old_current_size);
    assert(list.data == old_data);
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    /* Postcondition 4: If data was non-null before, it should still be non-null (clear doesn't free) */
    if (old_data != NULL) {
        assert(list.data != NULL);
    }

    /* Postcondition 5: If the list was zeroed before, it should still be zeroed */
    if (old_data == NULL && old_current_size == 0 && old_item_size == 0 && old_alloc == NULL) {
        assert(AWS_IS_ZEROED(list));
    }

    /* Cleanup */
    aws_array_list_clean_up(&list);
}
