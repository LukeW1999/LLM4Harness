#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_push_back_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.item_size > 0);

    uint8_t val_buf[MAX_ITEM_SIZE];
    const void *val = (const void *)val_buf;
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (old.data != NULL) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    int result = aws_array_list_push_back(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert(list.item_size == old.item_size);
        assert_bytes_match((const uint8_t *)list.data + (old.length * list.item_size),
                           (const uint8_t *)val,
                           list.item_size);
        assert(list.alloc >= old.alloc);
        assert(list.current_size >= old.current_size);
    } else {
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (list.data != NULL) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
