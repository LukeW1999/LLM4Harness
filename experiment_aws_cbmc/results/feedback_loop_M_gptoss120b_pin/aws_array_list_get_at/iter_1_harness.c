#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Allocate a readable buffer for val */
    /* item_size is guaranteed non‑zero by the validity predicate */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* optionally nondet fill the buffer – not required for verification */

    /* 4. Nondeterministic index, bounded */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* validity must hold */
        assert(aws_array_list_is_valid(&list));

        /* fields that never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* length update semantics */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* the stored element must equal the input value */
        assert_bytes_match(
            (uint8_t *)list.data + (index * list.item_size),
            val,
            list.item_size);
    } else {
        /* on failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        /* validity must still hold */
        assert(aws_array_list_is_valid(&list));
    }

    /* 7. Result must be a known outcome */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
