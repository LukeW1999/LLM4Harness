#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data = {0};
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_data);
    }

    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = aws_mem_acquire(allocator, list.item_size);
        __CPROVER_assume(val != NULL);
    }

    size_t index = nondet_size_t();
    if (list.item_size > 0) {
        size_t max_index = MAX_BUFFER_SIZE / list.item_size;
        __CPROVER_assume(index <= max_index);
    } else {
        __CPROVER_assume(index == 0);
    }

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));
        size_t expected_len = (old.length > index) ? old.length : index + 1;
        assert(list.length == expected_len);
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size >= old.current_size);
    } else {
        assert(aws_array_list_is_valid(&list));
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        if (list.data) {
            assert_byte_from_buffer_matches(list.data, &old_data);
        }
    }
}
