#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_push_front_harness() {
    struct aws_array_list list;
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    struct aws_allocator *alloc = (struct aws_allocator *)nondet_voidp();
    __CPROVER_assume(item_size > 0 && initial_item_allocation > 0);
    __CPROVER_assume(aws_array_list_init(&list, alloc, initial_item_allocation, item_size) == AWS_OP_SUCCESS);
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old_list = list;
    uint8_t *old_data = (uint8_t *)list.data;

    uint8_t val[item_size];
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    int result = aws_array_list_push_front(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length + 1);
        assert(list.data != old_data || old_list.length == 0); // If length was 0, data might not change
        assert(memcmp(old_data, list.data + list.item_size, old_list.length * list.item_size) == 0);
        assert(memcmp(list.data, val, list.item_size) == 0);
    } else {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_data);
    }

    assert(aws_array_list_is_valid(&list));
    aws_array_list_clean_up(&list);
}
