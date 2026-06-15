#include <stdlib.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_get_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable output buffer of the appropriate item size */
    size_t item_sz = list.item_size;               /* validity guarantees > 0 */
    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 3. Save a byte from the output buffer for immutability checks */
    struct store_byte_from_buffer old_val_bytes;
    save_byte_from_array(val, item_sz, &old_val_bytes);

    /* 4. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Save old state of the list for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_list_bytes;
    if (list.data && list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_list_bytes);
    }

    /* 6. Call the function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* 7. The array list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* ---- Success path postconditions ---- */

        /* The index must be within bounds */
        assert(index < list.length);

        /* The output buffer now contains the element at the given index */
        if (list.data) {
            assert_bytes_match((uint8_t *)list.data + index * list.item_size,
                               val,
                               list.item_size);
        }

        /* The list structure must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        if (list.data && list.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_list_bytes);
        }
    } else {
        /* ---- Failure path postconditions ---- */

        /* The output buffer must be unchanged */
        assert_byte_from_buffer_matches(val, &old_val_bytes);

        /* The list structure must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        if (list.data && list.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_list_bytes);
        }
    }

    /* Clean up */
    free(val);
}
