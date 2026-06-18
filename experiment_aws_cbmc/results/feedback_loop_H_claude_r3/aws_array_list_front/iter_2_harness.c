#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_front_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Create a non-deterministic output pointer */
    void *val;

    /* 3. Save old state before calling */
    struct aws_array_list old_list = list;

    /* 4. Call function under test */
    int result = aws_array_list_front(&list, &val);

    /* 5. Assert postconditions */

    /* Validity invariant must always hold */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must NOT change regardless of result */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    assert(list.length == old_list.length);
    assert(list.current_size == old_list.current_size);
    assert(list.data == old_list.data);

    if (result == AWS_OP_SUCCESS) {
        /* On success, list must have had at least one element */
        assert(old_list.length > 0);
    } else {
        /* On failure, list must have been empty */
        assert(old_list.length == 0);
    }
}
