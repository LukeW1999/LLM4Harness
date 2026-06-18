#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_push_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    if (list.alloc != NULL) {
        list.alloc = allocator;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t val[MAX_ITEM_SIZE];
    __CPROVER_havoc_object(val);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_byte);

    int result = aws_array_list_push_back(&list, val);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        size_t old_length_plus_one = 0;
        assert(aws_add_size_checked(old.length, 1, &old_length_plus_one) == AWS_OP_SUCCESS);

        assert(list.length == old_length_plus_one);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size >= old.current_size);

        size_t offset = 0;
        size_t end = 0;
        assert(aws_mul_size_checked(old.length, list.item_size, &offset) == AWS_OP_SUCCESS);
        assert(aws_add_size_checked(offset, list.item_size, &end) == AWS_OP_SUCCESS);
        assert(end <= list.current_size);

        assert_bytes_match((const uint8_t *)list.data + offset, val, list.item_size);

        if (old.alloc == NULL) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
            assert(end <= old.current_size);
        } else {
            size_t old_capacity = old.current_size / old.item_size;
            if (old.length < old_capacity) {
                assert(list.data == old.data);
                assert(list.current_size == old.current_size);
            }
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    assert(aws_array_list_is_valid(&list));
}
