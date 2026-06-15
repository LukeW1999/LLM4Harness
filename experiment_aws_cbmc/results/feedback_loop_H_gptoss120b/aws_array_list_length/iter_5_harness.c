#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data_byte = {0};
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_data_byte);
    }

    /* 3. Prepare a readable input value */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Choose a nondeterministic index */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < (SIZE_MAX / list.item_size)); /* avoid overflow */

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Global post‑condition: the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* ----- Success path ----- */
        size_t expected_length = old.length;
        if (index >= old.length) {
            expected_length = index + 1;
        }
        assert(list.length == expected_length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        uint8_t *dest = (uint8_t *)list.data + (index * list.item_size);
        assert_bytes_match(dest, val, list.item_size);
    } else {
        /* ----- Failure path ----- */
        assert_array_list_equivalence(&list, &old, &old_data_byte);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    }

    /* clean up */
    free(val);
}
