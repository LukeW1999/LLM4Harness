#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_front_harness() {
    struct aws_array_list list;

    /* Bound the array list to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION,
                                                MAX_ITEM_SIZE));

    /* Allocate the data member non-deterministically */
    ensure_array_list_has_allocated_data_member(&list);

    /* Set a valid allocator (required for validity) */
    list.alloc = aws_default_allocator();

    /* Assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save the pre‑call state */
    struct aws_array_list old = list;

    /* Call the function under test */
    int rval = aws_array_list_pop_front(&list);

    if (rval == AWS_OP_SUCCESS) {
        /* Success implies the list was non‑empty */
        assert(old.length > 0);

        /* Exactly one element removed from the front */
        assert(list.length == old.length - 1);

        /* These fields should not be modified by a pop operation */
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* Verify that the remaining elements were correctly shifted left:
         * the old data starting at byte offset item_size (i.e., old_data + item_size)
         * now resides at the beginning of new_data. */
        size_t new_len = list.length;
        size_t item_sz = list.item_size;
        uint8_t *old_data = (uint8_t *)old.data;
        uint8_t *new_data = (uint8_t *)list.data;
        if (new_len > 0) {
            assert_bytes_match(new_data, old_data + item_sz,
                               new_len * item_sz);
        }
    } else {
        /* Failure path: list was empty and an error was raised */
        assert(rval != AWS_OP_SUCCESS);
        assert(old.length == 0);

        /* The list must remain completely unchanged */
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
    }

    /* The structural invariants of the list must still hold */
    assert(aws_array_list_is_valid(&list));
}
