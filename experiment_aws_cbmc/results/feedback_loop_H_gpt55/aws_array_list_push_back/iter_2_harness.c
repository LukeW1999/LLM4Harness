#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_push_back_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    struct aws_allocator *allocator = aws_default_allocator();
    if (nondet_bool()) {
        list.alloc = allocator;
    } else {
        list.alloc = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t val[MAX_ITEM_SIZE];
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    size_t old_capacity = old.current_size / old.item_size;

    struct store_byte_from_buffer old_byte;
    save_byte_from_array(old.data, old.current_size, &old_byte);

    int result = aws_array_list_push_back(&list, val);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (old.length < old_capacity) {
        assert(result == AWS_OP_SUCCESS);
    }

    if (old.alloc == NULL && old.length == old_capacity) {
        assert(result == AWS_OP_ERR);
    }

    if (result == AWS_OP_SUCCESS) {
        size_t expected_length = old.length + 1;
        size_t appended_offset = old.length * old.item_size;
        size_t required_size = expected_length * old.item_size;

        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == expected_length);
        assert(list.current_size >= required_size);
        assert(list.data != NULL);

        assert_bytes_match((const uint8_t *)list.data + appended_offset, val, old.item_size);

        if (old.length < old_capacity) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        }

        if (old.alloc == NULL) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
}
