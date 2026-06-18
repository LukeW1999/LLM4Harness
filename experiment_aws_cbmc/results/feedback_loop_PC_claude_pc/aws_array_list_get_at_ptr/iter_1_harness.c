#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_get_at_ptr_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Output pointer */
    void *val = NULL;

    /* 4. Save old state before the call */
    struct aws_array_list old_list = list;

    /* 5. Call the function under test */
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* 6. Postconditions */

    /* Return value: success iff index < length */
    if (list.length > index) {
        /* Success path */
        assert(result == AWS_OP_SUCCESS);

        /* val must point to the correct element in the data buffer */
        assert(val != NULL);
        assert(val == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        /* Failure path */
        assert(result != AWS_OP_SUCCESS);
        /* val is unchanged (still NULL) on failure */
        assert(val == NULL);
    }

    /* Frame: all list fields must be unchanged */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* Invariant: list remains valid after the call */
    assert(aws_array_list_is_valid(&list));
}
