#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_ITEM_ALLOC, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list, allocator);
    size_t index;
    uint8_t val[list.item_size];
    __CPROVER_assume(index < list.length); // Ensure index is within bounds for success case

    struct aws_array_list old_list = list;
    int result = aws_array_list_get_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        // val should now contain the value at the specified index
    } else {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        // val should remain unchanged
    }

    assert(aws_array_list_is_valid(&list));
}
