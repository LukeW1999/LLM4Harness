#include <assert.h>
#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    aws_array_list_init(&list, allocator, MAX_INITIAL_CAPACITY, MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    struct store_byte_from_buffer old_data_byte;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_data_byte);
    }

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_BUFFER_SIZE);

    uint8_t val_buf[MAX_ITEM_SIZE];
    const void *val = val_buf;

    int result = aws_array_list_set_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));

    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        size_t expected_length = old.length;
        if (index >= old.length) {
            expected_length = index + 1;
        }
        assert(list.length == expected_length);
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val_buf,
                           list.item_size);
    } else {
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data != NULL && old.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_data_byte);
        }
    }

    aws_array_list_clean_up(&list);
}
