#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                                MAX_INITIAL_ITEM_ALLOCATION,
                                                MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data && list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    /* 3. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* length must decrease by one */
        assert(list.length == old.length - 1);

        /* capacity, data pointer, item size and allocator must stay the same */
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* the byte we saved (if it lies within the new length) must still match */
        if (old.length > 0 && old.data) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    } else {
        /* Failure case: index was out of bounds, list must be unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* the saved byte must still match */
        if (old.length > 0 && old.data) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }

        /* error must be due to invalid index */
        assert(index >= old.length);
    }

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
