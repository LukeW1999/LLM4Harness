#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

#ifndef MAX_ITEM_SIZE
#    define MAX_ITEM_SIZE 2
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#    define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

void aws_array_list_erase_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Save old state */
    struct aws_array_list old_list = list;

    /* Save a byte from the data for later comparison if needed */
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0 && list.data != NULL) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, index must have been valid (< length) */
        assert(index < old_list.length);

        /* Length decreases by 1 */
        assert(list.length == old_list.length - 1);

        /* item_size never changes */
        assert(list.item_size == old_list.item_size);

        /* alloc never changes */
        assert(list.alloc == old_list.alloc);

        /* data pointer never changes (erase doesn't reallocate) */
        assert(list.data == old_list.data);

        /* current_size never changes (erase doesn't reallocate) */
        assert(list.current_size == old_list.current_size);
    } else {
        /* On failure, index must have been out of bounds */
        assert(index >= old_list.length);

        /* Nothing should have changed */
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        assert(list.data == old_list.data);
        assert(list.current_size == old_list.current_size);
    }

    /* 6. Fields that never change regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    assert(list.data == old_list.data);
    assert(list.current_size == old_list.current_size);

    /* 7. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
