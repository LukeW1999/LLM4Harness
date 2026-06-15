#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness() {
    struct aws_array_list list;

    /* Bounding the data structures to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure the data member is properly allocated if current_size > 0 */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the operation */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save the old state of the list */
    struct aws_array_list old = list;

    /* Call the function under test */
    int result = aws_array_list_pop_back(&list);

    /* Assert postconditions based on success or failure */
    if (result == AWS_OP_SUCCESS) {
        /* On success, the list must have had at least one element */
        assert(old.length > 0);

        /* The length decreases by exactly one */
        assert(list.length == old.length - 1);

        /* The last element's memory must be zeroed */
        size_t last_offset = old.item_size * (old.length - 1);
        for (size_t i = 0; i < old.item_size; i++) {
            assert(((uint8_t *)list.data)[last_offset + i] == 0);
        }

        /* Unchanged fields: capacity (current_size), item_size, allocator, data pointer */
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
    } else {
        /* On failure, the list was empty (length == 0) */
        assert(old.length == 0);

        /* The list remains completely unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
    }

    /* The list invariant must hold after any possible execution */
    assert(aws_array_list_is_valid(&list));
}
