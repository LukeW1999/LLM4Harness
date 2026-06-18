#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for the value */
    size_t item_sz = list.item_size;
    __CPROVER_assume(item_sz > 0);               /* item_size is guaranteed non‑zero for a valid list */
    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    /* make the source buffer nondeterministic */
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();
    /* avoid overflow when computing offsets */
    __CPROVER_assume(item_sz == 0 || index <= SIZE_MAX / item_sz);

    /* 4. Save old state */
    struct aws_array_list old = list;
    /* Save a copy of the old data buffer for later comparison (if any) */
    uint8_t *old_data_copy = NULL;
    if (old.current_size > 0) {
        old_data_copy = malloc(old.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        for (size_t i = 0; i < old.current_size; ++i) {
            old_data_copy[i] = ((uint8_t *)old.data)[i];
        }
    }

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* The element at `index` must now equal `val` */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);

        /* Length updates */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* `current_size` may grow, so we do not assert it unchanged */
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        if (old.current_size > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               old_data_copy,
                               old.current_size);
        } else {
            assert(list.data == old.data);
        }
    }

    /* 7. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
    if (old_data_copy) {
        free(old_data_copy);
    }
}
