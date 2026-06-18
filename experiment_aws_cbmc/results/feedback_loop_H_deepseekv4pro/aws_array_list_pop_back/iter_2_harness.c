#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    /* 3. Call the function under test */
    int rval = aws_array_list_pop_back(&list);

    /* 4. Postconditions */
    assert(aws_array_list_is_valid(&list));

    if (rval == AWS_OP_SUCCESS) {
        /* Success: length decreased by 1, current_size unchanged, data pointer unchanged */
        assert(list.length == old.length - 1);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* The removed element is no longer part of the list, but the bytes in the buffer
         * beyond the new length should remain unchanged. */
        if (old.current_size > 0) {
            size_t new_length_bytes = list.length * list.item_size;
            if (old_byte.index < new_length_bytes) {
                assert_byte_from_buffer_matches(list.data, &old_byte);
            }
        }
    } else {
        /* Failure: list must be completely unchanged (empty list case) */
        assert(list.data == old.data);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        if (old.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }
}
