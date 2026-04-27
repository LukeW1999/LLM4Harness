#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_push_back_harness() {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    uint8_t *raw_array = can_fail_malloc(initial_item_allocation * item_size);
    __CPROVER_assume(raw_array != NULL);
    aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    struct aws_array_list old_list = list;
    uint8_t val[item_size];
    for (size_t i = 0; i < item_size; i++) {
        val[i] = nondet_uint8_t();
    }

    int result = aws_array_list_push_back(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length + 1);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        if (list.current_size > old_list.current_size) {
            // Reallocation happened
            assert(list.data != old_list.data);
        } else {
            // No reallocation
            assert(list.data == old_list.data);
        }
    } else {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(list.alloc == old_list.alloc);
    }

    assert(aws_array_list_is_valid(&list));
}
