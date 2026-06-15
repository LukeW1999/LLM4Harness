#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_sructures.h>

void aws_array_list_get_at_ptr_harness() {
    /* Data structure */
    struct aws_array_list list;
    size_t index;

    /* Boundedness and validity */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;

    /* Output pointer */
    void *val_ptr = NULL;
    void **val = &val_ptr;

    /* Non-deterministic index */
    __CPROVER_assume(index <= list.length);

    /* Call function under test */
    int result = aws_array_list_get_at_ptr(&list, val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, index must be within bounds */
        assert(index < list.length);
        /* On success, *val points to element at index */
        assert(*val == (void *)((uint8_t *)list.data + list.item_size * index));
    } else {
        /* On failure, index is out of bounds */
        assert(index >= list.length);
        /* On failure, result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }
    /* List is unchanged (const qualifier) */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
