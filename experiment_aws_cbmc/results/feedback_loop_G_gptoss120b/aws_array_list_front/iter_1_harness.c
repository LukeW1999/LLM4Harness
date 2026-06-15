#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_set_at_harness() {
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

    /* 3. Nondeterministic inputs */
    size_t index = nondet_size_t();
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        /* make the memory readable – any contents are fine */
    }

    /* 4. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Postconditions for success */
    if (result == AWS_OP_SUCCESS) {
        /* length is max(old.length, index+1) */
        size_t expected_len = (old.length > index) ? old.length : (index + 1);
        assert(list.length == expected_len);

        /* the element at index now matches the input value */
        if (list.data) {
            assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                               val,
                               list.item_size);
        }

        /* fields that must stay the same */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        /* 6. On failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        if (list.data && list.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    /* 7. Fields that never change regardless of result */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
