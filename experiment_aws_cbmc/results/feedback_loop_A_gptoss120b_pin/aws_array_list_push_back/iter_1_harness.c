#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source value */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;
    /* Save a copy of the old data buffer for later comparison on failure */
    uint8_t *old_data_copy = NULL;
    if (old.current_size > 0) {
        old_data_copy = malloc(old.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, old.data, old.current_size);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length must become max(old.length, index+1) */
        size_t expected_len = (index >= old.length) ? (index + 1) : old.length;
        assert(list.length == expected_len);

        /* The element at 'index' must now equal the source value */
        assert(list.data != NULL);
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index),
                           val,
                           list.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.current_size > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               old_data_copy,
                               old.current_size);
        }
    }

    /* 7. Fields that never change regardless of outcome */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 8. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
    free(old_data_copy);
}
