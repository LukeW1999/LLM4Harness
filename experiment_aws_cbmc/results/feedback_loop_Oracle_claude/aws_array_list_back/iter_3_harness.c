#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 4
#endif

void aws_array_list_back_harness(void) {
    struct aws_array_list list;

    /* Nondet initialize the list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Ensure item_size is non-zero to avoid issues */
    __CPROVER_assume(list.item_size > 0);

    /* Allocate val buffer of at least item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Ensure val does not overlap with list data - use CPROVER built-in */
    __CPROVER_assume(!__CPROVER_same_object(val, list.data));

    /* Save state before call for frame condition checks */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    int result = aws_array_list_back(&list, val);

    /* Postcondition 1: Return value correctness */
    if (old_length > 0) {
        /* List was non-empty: should succeed */
        assert(result == AWS_OP_SUCCESS);
    } else {
        /* List was empty: should return error */
        assert(result == AWS_OP_ERR);
    }

    /* Postcondition 2: List validity is preserved */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition 3: Frame conditions - list structure not modified */
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
    assert(list.alloc == old_alloc);

    /* Postcondition 4: Length invariants preserved */
    assert(list.length * list.item_size <= list.current_size);
}
