#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

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

    /* Non-deterministic input parameters */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    size_t index = nondet_size_t();

    /* Initialize list with some elements */
    size_t initial_length = nondet_size_t();
    __CPROVER_assume(initial_length <= list.current_size / list.item_size);
    list.length = initial_length;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(index < list.current_size / list.item_size); // Ensure index is within bounds for setting
        assert(list.length == (index >= old.length ? index + 1 : old.length));
        assert(AWS_MEM_IS_EQUAL(((uint8_t *)list.data) + (index * list.item_size), val, list.item_size));
    } else {
        assert(index >= list.current_size / list.item_size || !aws_array_list_is_valid(&list)); // Out of bounds or invalid list
        assert(list.length == old.length);
        assert(AWS_MEM_IS_EQUAL(list.data, old.data, old.current_size));
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    free(val);
}
