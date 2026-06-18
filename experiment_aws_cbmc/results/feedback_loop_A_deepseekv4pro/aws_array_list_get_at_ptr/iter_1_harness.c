#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_get_at_ptr_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Declare output pointer variable, save its initial value */
    void *item = NULL;               /* placeholder, will be overwritten on success */
    void **val_ptr = &item;
    void *old_val = item;

    /* 3. Choose a non-deterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old list state for immutability checks */
    struct aws_array_list old_list = list;

    /* 5. Call the function under test */
    int result = aws_array_list_get_at_ptr(&list, val_ptr, index);

    /* 6. Postconditions for list validity and immutability */
    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* 7. Branch on the expected outcome */
    if (aws_array_list_length(&list) > index) {
        /* Success path: element exists at index */
        assert(result == AWS_OP_SUCCESS);
        /* val_ptr must now point to the correct byte offset */
        assert(*val_ptr == (void *)((uint8_t *)list.data + list.item_size * index));
        /* The returned pointer must lie within the allocated data region */
        assert((uint8_t *)*val_ptr >= (uint8_t *)list.data &&
               (uint8_t *)*val_ptr < (uint8_t *)list.data + list.current_size);
    } else {
        /* Failure path: index out of bounds */
        assert(result == AWS_OP_ERR);
        /* val_ptr must remain unchanged */
        assert(*val_ptr == old_val);
    }
}
