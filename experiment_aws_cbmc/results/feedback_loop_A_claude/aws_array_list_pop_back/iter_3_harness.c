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
    void *val = malloc(list.item_size);
    /* val may or may not be NULL - non-deterministic */

    /* 3. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    void *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;

    /* 4. Call function under test */
    int result = aws_array_list_pop_back(&list, val);

    /* 5. Assert postconditions */

    /* item_size and alloc never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    /* List must always be valid after the call */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* On success, length must have decreased by 1 */
        assert(old_length > 0);
        assert(list.length == old_length - 1);
        /* current_size should not change */
        assert(list.current_size == old_current_size);
    } else {
        /* On failure, length should not have changed */
        assert(list.length == old_length);
        /* On failure, the list was empty */
        assert(old_length == 0);
    }
}
