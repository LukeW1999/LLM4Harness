#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_pop_back_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 3. Allocate a buffer to receive the popped value */
    void *val = NULL;
    if (old_item_size > 0) {
        val = malloc(old_item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 4. Call function under test */
    int result = aws_array_list_pop_back(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Fields that never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    assert(list.current_size == old_current_size);

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
