#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    size_t max_initial_item_allocation = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    __CPROVER_assume(max_initial_item_allocation > 0 && max_initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(max_item_size > 0 && max_item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_initial_item_allocation, max_item_size));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Non-deterministic input values */
    size_t index = nondet_size_t();
    char val[max_item_size];
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Check that the value at index is set correctly */
        char expected_val[max_item_size];
        memcpy(expected_val, val, list.item_size);
        assert(memcmp((char *)list.data + (list.item_size * index), expected_val, list.item_size) == 0);

        /* Check that length is updated if necessary */
        if (index >= old.length) {
            assert(list.length == old.length + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        /* On failure, the array list should be unchanged */
        assert(memcmp(&list, &old, sizeof(struct aws_array_list)) == 0);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* 7. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
