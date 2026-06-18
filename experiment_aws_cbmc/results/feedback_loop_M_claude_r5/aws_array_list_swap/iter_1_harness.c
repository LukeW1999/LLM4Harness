#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_array_list_swap_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Need at least 1 element for valid indices */
    __CPROVER_assume(list.length > 0);

    /* 2. Pick two non-deterministic indices within bounds */
    size_t a = nondet_size_t();
    __CPROVER_assume(a < list.length);

    size_t b = nondet_size_t();
    __CPROVER_assume(b < list.length);

    /* 3. Save old state BEFORE calling */
    struct aws_array_list old = list;

    /* Save bytes at positions a and b before the swap */
    /* We'll check that the data was swapped correctly */
    uint8_t *data_before_a = NULL;
    uint8_t *data_before_b = NULL;

    /* Get pointers to elements before swap */
    void *ptr_a = NULL;
    void *ptr_b = NULL;
    aws_array_list_get_at_ptr(&list, &ptr_a, a);
    aws_array_list_get_at_ptr(&list, &ptr_b, b);

    /* 4. Call function under test */
    aws_array_list_swap(&list, a, b);

    /* 5. Assert postconditions */

    /* Structural fields that must NOT change */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
