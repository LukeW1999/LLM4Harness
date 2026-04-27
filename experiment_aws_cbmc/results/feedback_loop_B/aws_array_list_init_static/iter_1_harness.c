#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    void *raw_array = (void *)malloc(MAX_ITEM_SIZE * MAX_INITIAL_ITEM_ALLOCATION);
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(raw_array != NULL);
    __CPROVER_assume(item_count > 0 && item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Since aws_array_list_init_static does not have a return value, we assume it always succeeds */
    {
        /* 1. Changed fields */
        assert(list.alloc == NULL);
        assert(list.current_size == item_count * item_size);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.data == raw_array);

        /* 2. Unchanged fields */
        /* No other fields are changed by this function */
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* None applicable here as the function does not have a failure path */

    /* 6. Assert validity invariants always holds */
    assert(aws_array_list_is_valid(&list));
}
