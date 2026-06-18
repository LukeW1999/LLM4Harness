#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_front_harness(void) {
    /* Set up the array list */
    struct aws_array_list list;

    /* Bound the list to prevent state space explosion */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Set up output buffer: must be item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    int result = aws_array_list_front(&list, val);

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* item_size must never change */
    assert(list.item_size == old_item_size);

    /* allocator pointer must never change */
    assert(list.alloc == old_alloc);

    /* length must never change (front is read-only) */
    assert(list.length == old_length);

    if (result == AWS_OP_SUCCESS) {
        /* Success requires the list to have been non-empty */
        assert(old_length > 0);
        /* val must have been written (data is non-null) */
        assert(list.data != NULL);
    } else {
        /* Failure means the list was empty */
        assert(old_length == 0);
    }
}
