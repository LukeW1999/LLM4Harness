#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = (uint8_t *)allocator->malloc(allocator, list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    size_t index = (size_t)nondet_uint64_t();

    struct aws_array_list old = list;
    uint8_t *old_data_copy = NULL;
    if (old.current_size > 0) {
        old_data_copy = (uint8_t *)allocator->malloc(allocator, old.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        aws_memcpy(old_data_copy, old.data, old.current_size);
    }

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        size_t expected_len = (index >= old.length) ? (index + 1) : old.length;
        assert(list.length == expected_len);
        assert(list.data != NULL);
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index),
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
