#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(list.alloc == NULL || list.alloc == allocator);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    uint8_t val[MAX_ITEM_SIZE];
    for (size_t i = 0; i < MAX_ITEM_SIZE; ++i) {
        val[i] = nondet_uint8_t();
    }
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_array_list old = list;

    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)old.data, old.current_size, &old_byte);

    int result = aws_array_list_get_at(&list, val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.alloc == old.alloc);
    assert_array_list_equivalence(&list, &old, &old_byte);

    if (index >= old.length) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
    }

    if (result == AWS_OP_SUCCESS) {
        size_t byte_offset = 0;
        int byte_offset_valid = aws_mul_size_checked(index, old.item_size, &byte_offset);
        assert(byte_offset_valid == AWS_OP_SUCCESS);

        if (byte_offset_valid == AWS_OP_SUCCESS) {
            assert_bytes_match(val, (const uint8_t *)old.data + byte_offset, old.item_size);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
