#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_back_harness(void) {
    /* 1. Declare and bound the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;

    /* 3. Allocate val buffer to receive the popped item */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL || list.item_size == 0);

    /* 4. Call function under test */
    int result = aws_array_list_pop_back(&list, val);

    /* 5. Assert postconditions */

    /* item_size must never change */
    assert(list.item_size == old_item_size);

    /* alloc must never change */
    assert(list.alloc == old_alloc);

    /* current_size must never change */
    assert(list.current_size == old_current_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the list must be valid */
        assert(aws_array_list_is_valid(&list));

        /* On success, length must have decreased by 1 */
        assert(list.length == old_length - 1);

        /* old_length must have been > 0 for success */
        assert(old_length > 0);

    } else {
        /* On failure, the list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* On failure, length should not have changed */
        assert(list.length == old_length);

        /* Failure implies the list was empty */
        assert(old_length == 0);
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
