#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* item_size must be > 0 for the function to be meaningful */
    __CPROVER_assume(list.item_size > 0);

    /* 2. Create a val buffer of item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    /* val must be readable for item_size bytes (precondition) */
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 3. Save old state */
    struct aws_array_list old_list = list;

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Assert postconditions */

    /* item_size must never change */
    assert(list.item_size == old_list.item_size);

    /* alloc must never change */
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length increases by 1 */
        assert(list.length == old_list.length + 1);

        /* current_size must be >= length * item_size */
        assert(list.current_size >= list.length * list.item_size);

        /* The first element in the list should now be val (data pointer must be valid) */
        assert(list.data != NULL);

        /* The list must still be valid */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure: length must not change */
        assert(list.length == old_list.length);

        /* current_size must not change */
        assert(list.current_size == old_list.current_size);

        /* The list must still be valid */
        assert(aws_array_list_is_valid(&list));
    }

    /* Regardless of result: item_size and alloc are unchanged (already asserted above) */
    /* current_size on success may grow, but on failure it stays the same */
    /* data pointer may change on success (realloc), but on failure it stays the same */
    if (result != AWS_OP_SUCCESS) {
        assert(list.data == old_list.data);
    }
}
