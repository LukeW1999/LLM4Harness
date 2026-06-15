#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    list.alloc = allocator;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + MAX_ITEM_SIZE);

    size_t item_sz = list.item_size;
    uint8_t val_buf[MAX_ITEM_SIZE];
    for (size_t i = 0; i < item_sz; ++i) {
        val_buf[i] = nondet_uint8_t();
    }

    int result = aws_array_list_set_at(&list, val_buf, index);

    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        uint8_t read_back[MAX_ITEM_SIZE];
        memcpy(read_back,
               (uint8_t *)list.data + (index * list.item_size),
               list.item_size);
        assert_bytes_match(read_back, val_buf, list.item_size);

        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (list.alloc == NULL) {
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }
}
