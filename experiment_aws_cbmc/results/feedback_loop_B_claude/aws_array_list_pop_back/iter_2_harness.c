#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_pop_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a buffer to receive the popped value */
    void *val = NULL;
    /* Non-deterministically decide whether to pass a valid buffer or NULL */
    uint8_t nondet_use_val;
    if (nondet_use_val && list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;

    /* 4. Call function under test */
    int result = aws_array_list_pop_back(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Fields that never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the list must have had at least one element */
        assert(old_length > 0);
        /* Length decreases by 1 */
        assert(list.length == old_length - 1);
    } else {
        /* On failure, the list was empty */
        assert(old_length == 0);
        /* Length unchanged */
        assert(list.length == old_length);
    }
}
