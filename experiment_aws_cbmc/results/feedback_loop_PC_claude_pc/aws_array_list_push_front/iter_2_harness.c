#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and initialize the array list */
    struct aws_array_list list;

    /* Use bounded initialization to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* 2. Create a val buffer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Assert postconditions */

    /* item_size must never change */
    assert(list.item_size == old_item_size);

    /* alloc must never change */
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length increases by 1 */
        assert(list.length == old_length + 1);

        /* current_size must be >= length * item_size */
        assert(list.current_size >= list.length * list.item_size);

        /* data pointer must be valid */
        assert(list.data != NULL);

        /* The list must still be valid */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure: length must not change */
        assert(list.length == old_length);

        /* current_size must not change */
        assert(list.current_size == old_current_size);

        /* data pointer must not change */
        assert(list.data == old_data);

        /* The list must still be valid */
        assert(aws_array_list_is_valid(&list));
    }
}
