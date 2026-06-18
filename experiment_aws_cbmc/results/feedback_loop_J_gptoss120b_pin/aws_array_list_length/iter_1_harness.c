#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

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

    /* Choose a nondeterministic index */
    size_t index = nondet_size_t();
    /* Keep the index within a reasonable range to avoid overflow */
    __CPROVER_assume(index <= old.length + 5);
    __CPROVER_assume(old.item_size != 0);
    __CPROVER_assume(index <= (SIZE_MAX / old.item_size));

    /* Save a byte from the element at the chosen index (if it exists) */
    struct store_byte_from_buffer old_byte_at_index;
    if (index < old.length) {
        save_byte_from_array((uint8_t *)old.data + index * old.item_size,
                             old.item_size,
                             &old_byte_at_index);
    }

    /* Save a byte from a different element to check unchanged memory */
    size_t other_index = nondet_size_t();
    __CPROVER_assume(old.length > 0);
    __CPROVER_assume(other_index < old.length);
    __CPROVER_assume(other_index != index);
    struct store_byte_from_buffer old_byte_other;
    save_byte_from_array((uint8_t *)old.data + other_index * old.item_size,
                         old.item_size,
                         &old_byte_other);

    /* Prepare a readable input value */
    uint8_t *val = malloc(old.item_size);
    __CPROVER_assume(val != NULL);
    /* The contents of *val are nondeterministic – no further assumption needed */

    /* 3. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length is updated only when writing past the previous end */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* The element at `index` now matches the input value */
        assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                           val,
                           list.item_size);

        /* Elements other than `index` are unchanged */
        if (old.length > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data +
                                            other_index * list.item_size,
                                            &old_byte_other);
        }

        /* Fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* On failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* If the index was within the old length, the original byte must be unchanged */
        if (index < old.length) {
            assert_byte_from_buffer_matches((uint8_t *)list.data +
                                            index * list.item_size,
                                            &old_byte_at_index);
        }
    }

    /* 5. The list must always be valid */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
}
