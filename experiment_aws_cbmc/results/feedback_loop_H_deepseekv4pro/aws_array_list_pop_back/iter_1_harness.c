#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate and assume readable memory for val */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Choose a bounded index */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(index < SIZE_MAX);                     /* avoid index+1 overflow */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(index <= SIZE_MAX / list.item_size);   /* avoid multiplication overflow */

    /* 4. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    /* 5. Call the function under test */
    int rval = aws_array_list_set_at(&list, val, index);

    /* 6. Postconditions */
    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    if (rval == AWS_OP_SUCCESS) {
        /* Success path: element at index must equal val */
        uint8_t *dest = (uint8_t *)list.data + (index * list.item_size);
        assert_bytes_match(val, dest, list.item_size);

        /* Length update: if index >= old length, new length = index + 1; else unchanged */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Allocator and item_size never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* Data buffer may have been reallocated; we cannot assert pointer equality.
         * Instead, check that bytes outside the modified element are unchanged. */
        if (old.current_size > 0) {
            size_t elem_start = index * list.item_size;
            size_t elem_end   = elem_start + list.item_size;
            if (old_byte.index < elem_start || old_byte.index >= elem_end) {
                assert_byte_from_buffer_matches(list.data, &old_byte);
            }
        }
    } else {
        /* Failure path: list must be completely unchanged */
        assert(list.data == old.data);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* Data content unchanged */
        if (old.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    /* Clean up */
    free(val);
}
