#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_pop_back_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a buffer to receive the popped element */
    void *val = NULL;
    /* Non-deterministically decide whether to pass a valid val or NULL */
    int use_val;
    if (use_val && list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    void *old_alloc = list.alloc;

    /* 4. Call function under test */
    int result = aws_array_list_pop_back(&list, val);

    /* 5. Assert postconditions */

    /* item_size and alloc never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the list must be valid */
        assert(aws_array_list_is_valid(&list));

        /* On success, length must have decreased by 1 */
        assert(old_length > 0);
        assert(list.length == old_length - 1);

    } else {
        /* On failure, the list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* On failure, length should not have changed */
        assert(list.length == old_length);
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
