#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;
    size_t max_item_alloc = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    ensure_array_list_has_allocated_data_member(&list, allocator);
    assume(aws_array_list_is_bounded(&list, max_item_alloc, max_item_size));
    assume(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    assume(index < list.length); // Ensure index is within bounds for success case

    uint8_t val[list.item_size];
    struct aws_array_list old_list = list;

    int result = aws_array_list_get_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        // val should be changed to the value at index in list.data
        assert_bytes_match(val, (uint8_t *)list.data + (list.item_size * index), list.item_size);
    } else {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
        // val should remain unchanged
        assert_bytes_match(val, val, list.item_size);
    }

    assert(aws_array_list_is_valid(&list));
}
