#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    size_t max_initial_item_allocation = 10;
    size_t max_item_size = 100;
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_initial_item_allocation, max_item_size));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Declare other variables */
    size_t index;
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    __CPROVER_assume(index < max_initial_item_allocation); // Ensure index is within bounds

    /* 3. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* 1. Changed fields */
        assert(AWS_MEM_EQ((uint8_t *)list.data + (list.item_size * index), val, list.item_size));

        /* 2. Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.length == old.length); // Length should not change with set_at
    } else {
        /* 1. Unchanged fields */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.length == old.length);
    }

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Free allocated memory */
    free(val);
}
