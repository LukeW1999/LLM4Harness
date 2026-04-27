#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_set_at_harness() {
    /* Setup: create a bounded array list */
    struct aws_array_list list;
    
    /* Ensure the list is valid before calling */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_current_size = list.current_size;

    /* Setup: create a valid val pointer */
    void *val = malloc(list.item_size > 0 ? list.item_size : 1);
    __CPROVER_assume(val != NULL);

    /* Choose a nondeterministic index - allow both in-bounds and out-of-bounds */
    size_t index;
    /* Don't restrict index too much - let CBMC explore */
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + 1);

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Frame conditions: item_size and alloc never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success, the list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* On success, data must not be NULL */
        assert(list.data != NULL);

        /* On success, length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* If index was >= old_length, length should now be index + 1 */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            /* length unchanged if index was within bounds */
            assert(list.length == old_length);
        }

    } else {
        /* On failure: length must be unchanged */
        assert(list.length == old_length);
        
        /* On failure: current_size must be unchanged */
        assert(list.current_size == old_current_size);
    }
    
    /* The result must be either success or failure */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
