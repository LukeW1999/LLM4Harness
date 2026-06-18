#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index = (size_t)nondet_uint64_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    uint8_t val_buf[MAX_ITEM_SIZE];
    for (size_t i = 0; i < list.item_size; ++i) {
        val_buf[i] = nondet_uint8_t();
    }
    const void *val = val_buf;

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.data != NULL) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    int result = aws_array_list_set_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            assert(list.length == index + 1);
        }
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val_buf,
                           list.item_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        if (old.data != NULL) {
            assert_byte_from_buffer_matches(old.data, &old_byte);
        } else {
            assert(list.data == NULL);
        }
    }

    aws_array_list_clean_up(&list);
}
