#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    list.alloc = aws_default_allocator();
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non‑deterministic index */
    size_t index = nondet_size_t();
    /* Keep index within a reasonable range to avoid overflow in calculations */
    __CPROVER_assume(index <= list.length + 5);

    /* 3. Allocate a readable buffer for val (must be non‑NULL) */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    } else {
        /* item_size == 0: allocate at least 1 byte to satisfy non‑NULL requirement */
        val = malloc(1);
        __CPROVER_assume(val != NULL);
    }

    /* 4. Save old state */
    struct aws_array_list old = list;
    size_t old_length = list.length;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        /* allocator and item size never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* length updates only when index is beyond the previous length */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }

        /* data pointer must be non‑NULL after a successful operation */
        assert(list.data != NULL);

        /* the stored element must match the input value */
        if (list.item_size > 0) {
            assert_bytes_match((uint8_t *)list.data + (list.item_size * index),
                               val,
                               list.item_size);
        }
    } else {
        /* On failure the list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 7. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
