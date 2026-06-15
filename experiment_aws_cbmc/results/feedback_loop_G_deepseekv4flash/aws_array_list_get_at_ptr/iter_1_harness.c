#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_ptr_harness() {
    /* Bounding parameters */
    size_t max_initial_item_allocation = nondet_size_t();
    __CPROVER_assume(max_initial_item_allocation < MAX_INITIAL_ITEM_ALLOCATION);
    size_t max_item_size = nondet_size_t();
    __CPROVER_assume(max_item_size < MAX_ITEM_SIZE);

    /* Declare and bound list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_initial_item_allocation, max_item_size));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state (list is const, but we copy for immutability check) */
    struct aws_array_list old_list = list;

    /* Nondeterministic inputs */
    size_t index = nondet_size_t();
    void *val;                      /* output pointer value */
    void **val_ptr = &val;          /* we will pass &val */

    int result = aws_array_list_get_at_ptr(&list, val_ptr, index);

    if (result == AWS_OP_SUCCESS) {
        assert(index < aws_array_list_length(&list));
        /* The output pointer should point to the element at index */
        assert(*val_ptr == (void *)((uint8_t *)list.data + list.item_size * index));
        /* Also ensure the address is non-null (since data is non-null for valid list) */
        assert(*val_ptr != NULL);
    } else {
        assert(index >= aws_array_list_length(&list));
        /* On failure, the output pointer is not modified; but we cannot assert it because it's uninitialized */
    }

    /* The list itself is const, so all fields remain unchanged */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
