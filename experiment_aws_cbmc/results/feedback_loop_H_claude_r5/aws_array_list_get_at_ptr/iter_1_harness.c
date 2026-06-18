#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_get_at_ptr_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old_list = list;

    /* 3. Non-deterministic index */
    size_t index;
    index = nondet_size_t();

    /* 4. Output pointer */
    void *val;

    /* 5. Call function under test */
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* 6. Assert postconditions */

    /* Success path: index < length, val points into the data buffer */
    if (result == AWS_OP_SUCCESS) {
        /* index must have been valid */
        assert(index < list.length);
        /* val should point to the correct offset in the data buffer */
        assert(val == (void *)((uint8_t *)list.data + (list.item_size * index)));
        /* val must be non-null (since data is valid and index is in bounds) */
        assert(val != NULL);
    } else {
        /* Failure path: index >= length */
        assert(index >= list.length);
    }

    /* 7. Unchanged fields — the list itself must not be modified */
    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);

    /* 8. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
