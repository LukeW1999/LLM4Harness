#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_capacity_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t capacity = aws_array_list_capacity(&list);

    /* Capacity must be at least the current length */
    assert(capacity >= list.length);

    if (list.item_size > 0) {
        /* Capacity is the number of items that fit in the allocated buffer */
        assert(capacity == list.current_size / list.item_size);
    } else {
        /* Zero-sized items imply zero capacity */
        assert(capacity == 0);
    }

    /* The list should remain valid after the call */
    assert(aws_array_list_is_valid(&list));
}
