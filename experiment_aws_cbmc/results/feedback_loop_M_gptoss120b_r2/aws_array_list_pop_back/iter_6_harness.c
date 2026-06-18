#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_pop_back_harness(void) {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Disallow zero‑item‑size when the list is non‑empty, which would make
     * the byte‑saving logic unsound. */
    __CPROVER_assume(!(list.length > 0 && list.item_size == 0));

    /* Strengthen the relationship between length, item_size and current_size. */
    __CPROVER_assume(list.current_size == list.length * list.item_size);

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    int result = aws_array_list_pop_back(&list);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        if (old.item_size != 0) {
            assert(list.current_size == old.current_size - old.item_size);
        } else {
            assert(list.current_size == old.current_size);
        }
        assert(list.data == old.data);
    } else {
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    if (list.data && old.item_size != 0 && old.current_size > 0) {
        if (old_byte.index < list.current_size) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
