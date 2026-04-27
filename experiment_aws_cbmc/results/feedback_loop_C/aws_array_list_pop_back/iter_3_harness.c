#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_array_list_pop_back_harness() {
    struct aws_array_list list;
    size_t max_item_alloc = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    struct aws_allocator *allocator = aws_default_allocator();

    ensure_array_list_has_allocated_data_member(&list, max_item_alloc, max_item_size, allocator);
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_item_alloc, max_item_size));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.length > 0); // Ensure the list is not empty

    struct aws_array_list old_list = list;

    int result = aws_array_list_pop_back(&list);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length - 1);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        // Check that the popped element is zeroed out
        uint8_t zeroed_element[max_item_size];
        memset(zeroed_element, 0, max_item_size);
        assert_bytes_match((uint8_t *)old_list.data + old_list.item_size * (old_list.length - 1), zeroed_element, old_list.item_size);
    } else {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        assert(list.data == old_list.data);
    }

    assert(aws_array_list_is_valid(&list));
}
