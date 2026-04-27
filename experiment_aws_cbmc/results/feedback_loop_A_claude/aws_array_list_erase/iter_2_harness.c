#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_erase_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Save old state */
    size_t old_length = list.length;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: index must have been valid (< old length) */
        assert(index < old_length);
        /* Length decreases by 1 */
        assert(list.length == old_length - 1);
        /* These fields must not change */
        assert(list.alloc == old_alloc);
        assert(list.item_size == old_item_size);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data);
    } else {
        /* On failure: index must have been out of bounds */
        assert(index >= old_length);
        /* List must be completely unchanged */
        assert(list.length == old_length);
        assert(list.alloc == old_alloc);
        assert(list.item_size == old_item_size);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data);
    }

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
