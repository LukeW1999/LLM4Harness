#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
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
    struct store_byte_from_buffer old_data;
    if (list.data) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_data);
    }

    /* 3. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Length must decrease by one */
        assert(list.length == old.length - 1);

        /* Fields that must not change */
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);

        if (list.data) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data,
                               list.current_size);
        }
    }

    /* 6. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
