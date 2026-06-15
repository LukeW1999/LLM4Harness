#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    size_t index = (size_t)nondet_uint64_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
        __CPROVER_assume(val != NULL);
    }

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
    }

    assert(aws_array_list_is_valid(&list));

    if (val) {
        aws_mem_release(allocator, val);
    }
}
