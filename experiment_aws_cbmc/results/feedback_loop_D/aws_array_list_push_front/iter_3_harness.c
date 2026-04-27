#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_push_front_harness() {
    struct aws_array_list list;
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    size_t max_initial_item_allocation = MAX_INITIAL_ITEM_ALLOCATION;
    size_t max_item_size = MAX_ITEM_SIZE;
    void *val = bounded_malloc(item_size);

    __CPROVER_assume(item_size > 0 && item_size <= max_item_size);
    __CPROVER_assume(initial_item_allocation <= max_initial_item_allocation);
    __CPROVER_assume(val != NULL);

    struct aws_allocator *allocator = aws_default_allocator();
    if (aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size) == AWS_OP_SUCCESS) {
        struct aws_array_list old_list = list;
        int result = aws_array_list_push_front(&list, val);

        if (result == AWS_OP_SUCCESS) {
            assert(list.length == old_list.length + 1);
            assert(memcmp(list.data, val, item_size) == 0);
            if (old_list.length > 0) {
                assert(memcmp((uint8_t *)list.data + item_size, old_list.data, old_list.length * item_size) == 0);
            }
        } else {
            assert(list.alloc == old_list.alloc);
            assert(list.current_size == old_list.current_size);
            assert(list.length == old_list.length);
            assert(list.item_size == old_list.item_size);
            assert(list.data == old_list.data);
        }

        assert(aws_array_list_is_valid(&list));
        aws_array_list_clean_up(&list);
    }
}
