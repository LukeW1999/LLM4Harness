#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t item_size = nondet_size_t();
    size_t initial_item_allocation = nondet_size_t();
    size_t length = nondet_size_t();
    size_t current_size = nondet_size_t();
    bool is_dynamic = nondet_bool();

    /* Bound the list to limit state space */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Initialize list in either dynamic or static mode */
    if (is_dynamic) {
        /* Dynamic mode: allocate data member */
        ensure_array_list_has_allocated_data_member(&list);
        __CPROVER_assume(aws_array_list_is_valid(&list));
    } else {
        /* Static mode: data is a pre-allocated array */
        list.alloc = NULL;
        list.data = malloc(item_size * initial_item_allocation);
        __CPROVER_assume(list.data != NULL);
        list.current_size = item_size * initial_item_allocation;
        list.length = 0;
        list.item_size = item_size;
    }

    /* Save old state */
    struct aws_array_list old = list;

    /* Non-deterministic value to push */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: length increased by 1 */
        assert(list.length == old.length + 1);
        /* Data pointer unchanged */
        assert(list.data == old.data);
        /* Item size unchanged */
        assert(list.item_size == old.item_size);
        /* Allocator unchanged */
        assert(list.alloc == old.alloc);
        /* Current size may have increased (if dynamic and needed resize) */
        if (is_dynamic) {
            assert(list.current_size >= old.current_size);
        } else {
            assert(list.current_size == old.current_size);
        }
    } else {
        /* Failure: list unchanged */
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
    if (!is_dynamic) {
        free(list.data);
    }
}
