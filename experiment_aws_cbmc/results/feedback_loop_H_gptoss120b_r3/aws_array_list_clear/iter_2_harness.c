#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    list.alloc = allocator;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = (uint8_t *)aws_mem_acquire(allocator,
                     list.item_size ? list.item_size : 1);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;

    size_t index = nondet_size_t();

    int result = aws_array_list_set_at(&list, val, index);

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        assert(list.current_size >= old.current_size);
        assert(list.length > index);
        assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                           val,
                           list.item_size);
    } else {
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    assert(aws_array_list_is_valid(&list));
}
