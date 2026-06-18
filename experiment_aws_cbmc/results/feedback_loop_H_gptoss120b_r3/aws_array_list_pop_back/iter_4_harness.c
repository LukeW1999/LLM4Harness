#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_array_list_pop_back_harness() {
    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Assume allocator function pointers are valid */
    __CPROVER_assume(allocator->mem_acquire != NULL);
    __CPROVER_assume(allocator->mem_calloc != NULL);
    __CPROVER_assume(allocator->mem_realloc != NULL);
    __CPROVER_assume(allocator->mem_release != NULL);

    struct aws_array_list list;
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* Initialise the list */
    aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, item_size);

    /* Ensure the list is within the expected bounds and valid */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, item_size));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Keep a copy of the original state */
    struct aws_array_list old = list;

    /* Perform the operation under test */
    int result = aws_array_list_pop_back(&list);

    /* Verify post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
    } else {
        assert(list.length == old.length);
    }
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    /* The list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    aws_array_list_clean_up(&list);
}
