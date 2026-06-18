#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Harness for aws_array_list_swap_contents
 *
 * Preconditions from implementation:
 *   - list_a->alloc != NULL
 *   - list_a->alloc == list_b->alloc
 *   - list_a->item_size == list_b->item_size
 *   - list_a != list_b
 *
 * Behavior: Swaps the entire contents of two dynamic array lists.
 *
 * Postconditions for BOTH lists:
 *   - alloc pointers remain unchanged (both point to same allocator)
 *   - item_size remains unchanged (it was already equal before)
 *   - All fields are swapped: current_size, length, data
 *   - After swap, both lists remain valid
 */
void aws_array_list_swap_contents_harness() {
    /* 1. Declare and build two array lists */
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    /* Bound and ensure data members */
    __CPROVER_assume(aws_array_list_is_bounded(&list_a, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list_a);
    ensure_array_list_has_allocated_data_member(&list_b);

    /* Ensure validity */
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* Enforce preconditions required by the implementation */
    __CPROVER_assume(list_a.alloc != NULL);
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    __CPROVER_assume(&list_a != &list_b);

    /* 2. Save old state */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* 3. Call function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* 4. Postconditions: fields that should have changed (swapped) */

    /* list_a now holds old_b's data fields */
    assert(list_a.current_size == old_b.current_size);
    assert(list_a.length == old_b.length);
    assert(list_a.data == old_b.data);

    /* list_b now holds old_a's data fields */
    assert(list_b.current_size == old_a.current_size);
    assert(list_b.length == old_a.length);
    assert(list_b.data == old_a.data);

    /* 5. Postconditions: fields that must NOT change */

    /* alloc pointers preserved (same allocator instance) */
    assert(list_a.alloc == old_a.alloc);
    assert(list_b.alloc == old_b.alloc);

    /* item_size preserved for both (they were equal before, remain equal) */
    assert(list_a.item_size == old_a.item_size);
    assert(list_b.item_size == old_b.item_size);

    /* alloc equality still holds */
    assert(list_a.alloc == list_b.alloc);

    /* item_size equality still holds */
    assert(list_a.item_size == list_b.item_size);

    /* 6. Validity invariants: both lists must remain valid */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
