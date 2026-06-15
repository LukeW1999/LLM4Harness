#include <stdlib.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable input buffer of the appropriate item size */
    size_t item_sz = list.item_size;               /* validity guarantees > 0 */
    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.data && list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_bytes);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. The array list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* ---- Success path postconditions ---- */

        /* The element at the given index now matches the input value */
        if (list.data) {
            assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                               val,
                               list.item_size);
        }

        /* Length is updated only if the index was beyond the previous length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* The data pointer may change in dynamic mode (reallocation), so we do not assert it. */
    } else {
        /* ---- Failure path postconditions ---- */

        /* The entire structure must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);

        /* The underlying buffer contents must be unchanged */
        if (list.data && list.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_bytes);
        }
    }

    /* Clean up */
    free(val);
}
