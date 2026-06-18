#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_push_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, 5, 8));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = (uint8_t *)allocator->malloc(allocator, list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    uint8_t *old_data_copy = NULL;
    if (old.current_size > 0) {
        old_data_copy = (uint8_t *)allocator->malloc(allocator, old.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        aws_memcpy(old_data_copy, old.data, old.current_size);
    }

    int result = aws_array_list_push_back(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert(list.data != NULL);
        assert_bytes_match((uint8_t *)list.data + (list.item_size * (list.length - 1)),
                           val,
                           list.item_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.current_size > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               old_data_copy,
                               old.current_size);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));

    allocator->free(allocator, val);
    if (old_data_copy != NULL) {
        allocator->free(allocator, old_data_copy);
    }
}
