#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_back_harness(void) {
    /* Allocate and initialize the array list */
    struct aws_array_list list;

    /* Use nondet values for item_size and length */
    size_t item_size;
    size_t initial_length;
    size_t initial_current_size;

    __CPROVER_assume(item_size > 0 && item_size <= 128);
    __CPROVER_assume(initial_length <= 8);
    __CPROVER_assume(initial_current_size >= initial_length * item_size);
    __CPROVER_assume(initial_current_size <= 1024);
    __CPROVER_assume(initial_current_size % item_size == 0);

    list.item_size = item_size;
    list.length = initial_length;
    list.current_size = initial_current_size;
    list.alloc = aws_default_allocator();

    if (initial_current_size > 0) {
        list.data = malloc(initial_current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
        __CPROVER_assume(initial_length == 0);
    }

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save state before the call */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    size_t old_item_size = list.item_size;

    /* Call the function under test */
    int result = aws_array_list_pop_back(&list);

    /* Postcondition 1: Return value correctness */
    if (old_length > 0) {
        assert(result == AWS_OP_SUCCESS);
    } else {
        assert(result == AWS_OP_ERR);
    }

    /* Postcondition 2: Length invariants */
    if (old_length > 0) {
        assert(list.length == old_length - 1);
    } else {
        assert(list.length == old_length);
    }

    /* Postcondition 3: Capacity (current_size) should not change */
    assert(list.current_size == old_current_size);

    /* Postcondition 4: item_size should not change */
    assert(list.item_size == old_item_size);

    /* Postcondition 5: data pointer should not change */
    assert(list.data == old_data);

    /* Postcondition 6: The list must still be valid after the call */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition 7: Frame condition - if list was empty, nothing changed */
    if (old_length == 0) {
        assert(list.length == 0);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data);
    }
}
