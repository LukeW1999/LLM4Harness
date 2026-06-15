#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
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

    /* 2. Non‑deterministic index */
    size_t index = nondet_size_t();

    /* 3. Allocate a readable source value */
    size_t item_sz = list.item_size;
    __CPROVER_assume(item_sz > 0);
    void *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_sz));

    /* Save a copy of the source bytes for later comparison */
    uint8_t *val_copy = malloc(item_sz);
    __CPROVER_assume(val_copy != NULL);
    memcpy(val_copy, val, item_sz);

    /* 4. Save old state before the call */
    struct aws_array_list old = list;
    struct aws_array_list old_before = old; /* for unchanged‑field checks */

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Length may increase to index+1 if we wrote past the old length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* The written element must match the source value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val_copy,
                           list.item_size);
    } else {
        /* On failure the list must remain unchanged */
        assert(list.length == old_before.length);
        assert(list.current_size == old_before.current_size);
        assert(list.data == old_before.data);
        assert(list.item_size == old_before.item_size);
        assert(list.alloc == old_before.alloc);
    }

    /* 7. Fields that never change regardless of success/failure */
    assert(list.item_size == old_before.item_size);
    assert(list.alloc == old_before.alloc);

    /* 8. Validity invariant must hold after the call */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
    free(val_copy);
}
