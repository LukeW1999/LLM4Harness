#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old_list = list;
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;

    /* 3. Allocate output buffer for front element */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL || list.item_size == 0);

    /* 4. Call function under test */
    int result = aws_array_list_front(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant always holds after the call */
    assert(aws_array_list_is_valid(&list));

    /* item_size and alloc never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_list.alloc);

    /* length never changes for front (read-only operation) */
    assert(list.length == old_length);

    /* current_size never changes for front (read-only operation) */
    assert(list.current_size == old_list.current_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success: list must have had at least one element */
        assert(old_length > 0);

        /* data must be non-null since list had elements */
        assert(list.data != NULL);

        /* val must have been written (readable) */
        if (list.item_size > 0) {
            assert(AWS_MEM_IS_READABLE(val, list.item_size));
        }
    } else {
        /* On failure: list was empty */
        assert(old_length == 0);
    }
}
