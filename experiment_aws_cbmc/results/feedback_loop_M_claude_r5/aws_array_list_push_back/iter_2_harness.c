#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_push_back_harness(void) {
    /* 1. Set up the array list */
    struct aws_array_list list;

    /* Bound the list to prevent state space explosion */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Set up val - must be readable for item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* 3. Save old state */
    struct aws_array_list old_list = list;

    /* 4. Call the function under test */
    int result = aws_array_list_push_back(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* item_size never changes */
    assert(list.item_size == old_list.item_size);

    /* alloc never changes */
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length must have increased by 1 */
        assert(list.length == old_list.length + 1);

        /* current_size must be sufficient to hold the data */
        assert(list.current_size >= list.length * list.item_size);

        /* data must be non-null since we have elements */
        assert(list.data != NULL);

        /* The value at the last index must match val */
        assert_bytes_match((uint8_t *)list.data + (list.item_size * old_list.length), val, list.item_size);

    } else {
        /* On failure: list must still be valid (already asserted above) */
        /* In static mode with full list, list is unchanged */
        if (list.alloc == NULL) {
            /* Static mode: list should be unchanged */
            assert(list.length == old_list.length);
            assert(list.current_size == old_list.current_size);
            assert(list.data == old_list.data);
        }
    }
}
