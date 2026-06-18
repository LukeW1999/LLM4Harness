#include <assert.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = (uint8_t *)aws_mem_acquire(allocator, list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    size_t index = nondet_size_t();

    struct aws_array_list old = list;

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            assert(list.length == index + 1);
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    assert(aws_array_list_is_valid(&list));

    aws_mem_release(allocator, val);
}
