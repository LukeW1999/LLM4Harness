/* Harness for aws_array_list_push_front */
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Allocate a nondeterministic input value */
    size_t item_sz = list.item_size;
    /* item size must be non‑zero for a meaningful push */
    __CPROVER_assume(item_sz > 0);
    uint8_t *val = (uint8_t *)malloc(item_sz);
    __CPROVER_assume(val != NULL);
    /* make the buffer nondeterministically initialized */
    for (size_t i = 0; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length must increase by one */
        assert(list.length == old.length + 1);

        /* the new front element must equal the supplied value */
        assert_bytes_match((const uint8_t *)list.data, val, item_sz);

        /* fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* current_size may change (reallocation), data pointer may change,
           so we do not assert them here. */
    } else {
        /* on failure the whole structure must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 6. The list must still be valid */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
