#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate output buffer of item_size bytes */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Save old state */
    struct aws_array_list old_list = list;

    /* 4. Call function under test */
    int result = aws_array_list_front(&list, val);

    /* 5. Assert postconditions */

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Fields that never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.data == old_list.data);

    if (result == AWS_OP_SUCCESS) {
        /* Success means the list was non-empty */
        assert(old_list.length > 0);
        /* val should have been written - data must be non-null */
        assert(list.data != NULL);
    } else {
        /* Failure means the list was empty */
        assert(old_list.length == 0);
    }
}
