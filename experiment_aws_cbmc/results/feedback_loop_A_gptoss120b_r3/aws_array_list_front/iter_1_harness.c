#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non‑deterministic index, bounded to avoid overflow in multiplication */
    size_t index = nondet_size_t();
    if (list.item_size != 0) {
        __CPROVER_assume(index <= SIZE_MAX / list.item_size);
    }

    /* 3. Allocate a readable source buffer for the value to set */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    /* 4. Save old state */
    struct aws_array_list old = list;
    /* Save a copy of the old data buffer for later comparison (if any) */
    uint8_t *old_data_copy = NULL;
    if (old.data && old.current_size > 0) {
        old_data_copy = malloc(old.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, old.data, old.current_size);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Global invariant must hold */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* ----- Success path ----- */

        /* The element at the given index now matches the source value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           (uint8_t *)val,
                           list.item_size);

        /* Length changes only when the index was at or beyond the old length */
        if (index >= old.length) {
            assert(list.length == old.length + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Fields that never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* All other elements (except possibly the one at `index`) remain unchanged */
        for (size_t i = 0; i < old.length; ++i) {
            if (i == index) {
                continue;
            }
            assert_bytes_match((uint8_t *)list.data + (i * list.item_size),
                               (uint8_t *)old_data_copy + (i * old.item_size),
                               list.item_size);
        }
    } else {
        /* ----- Failure path ----- */

        /* The list must be unchanged */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data && old.current_size > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old_data_copy,
                               list.current_size);
        }

        /* Validity invariant still holds */
        assert(aws_array_list_is_valid(&list));
    }

    /* Clean up auxiliary allocations */
    free(val);
    free(old_data_copy);
}
