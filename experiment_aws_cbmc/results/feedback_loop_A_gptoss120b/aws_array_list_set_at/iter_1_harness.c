#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

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

    /* 3. Prepare a readable input value */
    size_t item_size = list.item_size;
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    uint8_t *val_bytes = (uint8_t *)val;
    for (size_t i = 0; i < item_size; ++i) {
        val_bytes[i] = nondet_uint8_t();
    }

    /* Save a copy of the input value for later comparison */
    uint8_t *saved_val = malloc(item_size);
    __CPROVER_assume(saved_val != NULL);
    memcpy(saved_val, val, item_size);

    /* 4. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length is updated to max(old.length, index+1) */
        size_t expected_length = (index >= old.length) ? (index + 1) : old.length;
        assert(list.length == expected_length);

        /* The element at the given index now matches the input value */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index),
                           saved_val,
                           list.item_size);
    } else {
        /* On failure the length must not have changed */
        assert(list.length == old.length);
    }

    /* 7. Fields that must remain unchanged regardless of outcome */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 8. The list must remain valid */
    assert(aws_array_list_is_valid(&list));
}
