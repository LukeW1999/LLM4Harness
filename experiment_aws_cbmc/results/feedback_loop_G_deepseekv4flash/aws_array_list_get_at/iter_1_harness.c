#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_set_at_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t index;

    /* Bound data structures to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state for comparison */
    struct aws_array_list old = list;

    /* Non-deterministic value buffer of appropriate size */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Bound index to avoid state explosion */
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions on success */
    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            /* Length increased to index+1 */
            assert(list.length == index + 1);
        } else {
            /* Length unchanged */
            assert(list.length == old.length);
        }
        /* Data at index is overwritten with val */
        assert(index < list.length);
        assert_bytes_match((uint8_t *)list.data + index * list.item_size, val, list.item_size);
    }
    /* On failure, no specific changes are guaranteed; list remains valid. */

    /* Field immutability (fields not modified by the function) */
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
