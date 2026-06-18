#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_get_at_ptr_harness() {
    /* Declare and initialise the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state for checks */
    struct aws_array_list old_list = list;
    struct store_byte_from_buffer byte_storage;
    /* Only save a byte if there is allocated data */
    if (list.data != NULL && list.current_size * list.item_size > 0) {
        save_byte_from_array((const uint8_t *)list.data, list.current_size * list.item_size, &byte_storage);
    }

    /* Output pointer – capture initial value */
    void *val;
    void *val_old = val;

    /* Non-deterministic index */
    size_t index = nondet_size_t();

    /* Call the function under verification */
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: element exists and val points to it */
        assert(index < list.length);
        assert(val == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* Failure: element does not exist, val untouched */
        assert(index >= list.length);
        assert(val == val_old);
    }

    /* The list is const – nothing should change */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);
    if (old_list.data != NULL) {
        /* Verify that the buffer contents are untouched */
        assert_byte_from_buffer_matches((const uint8_t *)list.data, &byte_storage);
    }

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
