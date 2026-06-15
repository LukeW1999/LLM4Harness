#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    size_t index = nondet_size_t();

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.data) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size >= old.current_size);
    } else {
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data) {
            assert_byte_from_buffer_matches(old.data, &old_byte);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
