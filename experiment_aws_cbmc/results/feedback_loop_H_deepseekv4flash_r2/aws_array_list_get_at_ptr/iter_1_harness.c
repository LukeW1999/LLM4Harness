#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_get_at_ptr_harness() {
    /* Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* Declare output pointer and save its initial value */
    void *val_ptr;
    void *old_val_ptr = val_ptr;

    /* Non-deterministic index */
    size_t index = nondet_size_t();

    /* Call the function */
    int result = aws_array_list_get_at_ptr(&list, &val_ptr, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, *val_ptr must point to the correct element */
        assert((uint8_t *)val_ptr == (uint8_t *)list.data + list.item_size * index);
    } else {
        /* On failure, *val_ptr must be unchanged */
        assert(val_ptr == old_val_ptr);
    }

    /* Unchanged fields: all fields of list are const, so they must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* Data content must be unchanged */
    assert_array_list_equivalence(&list, &old, &old_byte);

    /* Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
