#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
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
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0); /* validity requires non‑zero item size */

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Prepare inputs */
    /* nondeterministic index, bounded to avoid overflow in our checks */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(list.item_size == 0 || index <= SIZE_MAX / list.item_size);

    /* allocate a readable buffer for val */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* allocator and item size never change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        /* length either stays the same or grows to index+1 */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* current size never shrinks */
        assert(list.current_size >= old.current_size);

        /* data pointer must be non‑NULL after successful ensure_capacity */
        assert(list.data != NULL);

        /* the element at the given index now matches the input value */
        assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                           val,
                           list.item_size);
    } else {
        /* On failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 6. Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
}
