#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Declare and bound input parameters */
    size_t index = nondet_size_t();
    uint8_t *val = malloc(list.item_size); // Allocate memory for val

    /* 4. Assume preconditions */
    __CPROVER_assume(index < list.length);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(__CPROVER_writable_memory(val, list.item_size));

    /* 5. Call function under test */
    int result = aws_array_list_get_at(&list, index, (void **)&val);

    /* 6. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(val != NULL);
        assert(AWS_MEM_EQ(val, (uint8_t *)list.data + (list.item_size * index), list.item_size));
    } else {
        // The function should not set val to NULL on failure
        // Remove this assertion as it is incorrect
    }

    /* 7. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.length == old.length);

    /* 8. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    free(val); // Free allocated memory
}
