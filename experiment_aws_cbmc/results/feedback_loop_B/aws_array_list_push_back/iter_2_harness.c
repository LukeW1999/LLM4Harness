#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_push_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Create and bound the item to be pushed */
    char item[MAX_ITEM_SIZE];
    __CPROVER_assume(AWS_MEM_IS_READABLE(item, list.item_size));

    /* 4. Call function under test */
    int result = aws_array_list_push_back(&list, item);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Check that the new length is incremented */
        assert(list.length == old.length + 1);

        /* Check that the item is added at the end of the list */
        char actual_item[MAX_ITEM_SIZE];
        memcpy(actual_item, (void *)((uint8_t *)list.data + (list.item_size * (old.length))), list.item_size);
        assert_bytes_match(actual_item, item, list.item_size);

        /* Check that other fields remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.current_size >= old.current_size);
        assert(list.item_size == old.item_size);
    } else {
        /* On failure, the list should remain unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    }

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
