#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_clean_up_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 3. Assert postconditions:
     *    The function calls AWS_ZERO_STRUCT(*list), which zeroes all fields.
     *    After clean_up:
     *      - alloc should be NULL
     *      - current_size should be 0
     *      - length should be 0
     *      - item_size should be 0
     *      - data should be NULL
     */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    assert(list.data == NULL);

    /* 4. Assert validity invariant holds after clean_up.
     *    A zeroed list (with data == NULL and current_size == 0) is valid. */
    assert(aws_array_list_is_valid(&list));
}
