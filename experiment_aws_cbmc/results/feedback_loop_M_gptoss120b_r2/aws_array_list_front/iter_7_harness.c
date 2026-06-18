#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 5
#define MAX_ITEM_SIZE 256

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    list.allocator = allocator;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t val_buf[MAX_ITEM_SIZE];
    __CPROVER_assume(list.item_size <= sizeof(val_buf));
    for (size_t i = 0; i < list.item_size; ++i) {
        val_buf[i] = nondet_uint8_t();
    }

    size_t index = (size_t)nondet_uint64_t();
    __CPROVER_assume(index <= (MAX_INITIAL_ITEM_ALLOCATION * 2));
    __CPROVER_assume(list.item_size == 0 ||
                     index <= (SIZE_MAX / list.item_size));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (old.data && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    int result = aws_array_list_set_at(&list, val_buf, index);

    if (result == AWS_OP_SUCCESS) {
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val_buf,
                           list.item_size);
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data && old.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
