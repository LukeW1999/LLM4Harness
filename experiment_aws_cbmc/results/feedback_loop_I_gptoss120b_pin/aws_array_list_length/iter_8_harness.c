#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    size_t item_sz = list.item_size;
    __CPROVER_assume(item_sz <= MAX_ITEM_SIZE);
    uint8_t val_buf[MAX_ITEM_SIZE];
    for (size_t i = 0; i < item_sz; ++i) {
        val_buf[i] = nondet_uint8_t();
    }

    size_t index = nondet_size_t();

    int result = aws_array_list_set_at(&list, val_buf, index);

    if (result == AWS_OP_SUCCESS) {
        assert(index < list.length);
        assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                           val_buf,
                           list.item_size);
        size_t expected_len = (old.length > index) ? old.length : (index + 1);
        assert(list.length == expected_len);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (list.data && list.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
}
