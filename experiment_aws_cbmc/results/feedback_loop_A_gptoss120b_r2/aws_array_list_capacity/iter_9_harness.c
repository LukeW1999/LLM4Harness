#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    size_t item_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(item_size > 0 && item_size < 1024);
    size_t initial_capacity = (size_t)nondet_uint64_t();
    __CPROVER_assume(initial_capacity < 1024);

    int init_res = aws_array_list_init(&list, allocator, initial_capacity, item_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
        __CPROVER_assume(val != NULL);
    }

    size_t index = (size_t)nondet_uint64_t();

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            assert(list.length == old.length + 1);
        } else {
            assert(list.length == old.length);
        }
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size >= old.current_size);
    } else {
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    assert(aws_array_list_is_valid(&list));

    if (val != NULL) {
        aws_mem_release(allocator, val);
    }
    aws_array_list_clean_up(&list);
}
