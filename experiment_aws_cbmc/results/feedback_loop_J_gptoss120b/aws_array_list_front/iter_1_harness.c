#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* 2. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer storage; /* for equivalence checks */

    /* 3. Nondeterministic index (bounded to keep arithmetic safe) */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= SIZE_MAX / list.item_size);
    __CPROVER_assume(index < (MAX_INITIAL_ITEM_ALLOCATION * 2U));

    /* 4. Allocate and initialise val (readable memory) */
    uint8_t *val = (uint8_t *)malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* Keep a copy of the input bytes for later comparison */
    uint8_t *val_copy = (uint8_t *)malloc(list.item_size);
    __CPROVER_assume(val_copy != NULL);
    memcpy(val_copy, val, list.item_size);

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The element at `index` must now contain the bytes from `val` */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val_copy,
                           list.item_size);

        /* Length updates */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        /* On failure the list must be unchanged */
        assert_array_list_equivalence(&list, &old, &storage);
    }

    /* 7. Fields that never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
    free(val_copy);
}
