#include <assert.h>
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
    __CPROVER_assume(list.item_size > 0);               /* non‑zero item size */

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.data != NULL && list.current_size > 0) {
        size_t offset = nondet_size_t();
        __CPROVER_assume(offset < list.current_size);
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 3. Prepare inputs */
    size_t index = nondet_size_t();

    size_t val_size = list.item_size;
    void *val = malloc(val_size);
    __CPROVER_assume(val != NULL);                     /* val must be readable */

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success is only possible when index is within the current length */
        assert(index < old.length);
        /* Data pointer must stay the same */
        assert(list.data == old.data);
        /* The element at the given index now matches the input value */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index),
                           (uint8_t *)val,
                           list.item_size);
        /* Length does not change on a successful set */
        assert(list.length == old.length);
    } else {
        /* On failure the whole list must remain unchanged */
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    /* 6. Fields that must never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);

    /* 7. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
}
