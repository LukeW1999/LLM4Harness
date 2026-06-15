#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_array_list_erase_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t index;

    /* Bound the list and ensure data member is allocated */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* Call the function */
    int result = aws_array_list_erase(&list, index);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, length decreases by 1 */
        assert(list.length == old.length - 1);
        /* current_size and data pointer remain unchanged */
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        /* item_size and alloc remain unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        /* For non-empty list, data content may have shifted but we cannot fully verify without knowing index */
        /* At minimum, the list is still valid */
    } else {
        /* On failure (index >= old.length), list is unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
