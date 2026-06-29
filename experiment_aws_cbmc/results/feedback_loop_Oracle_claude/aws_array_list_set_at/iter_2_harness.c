#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
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

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;

    /* Initialize with a valid item_size */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* Initialize the list manually to ensure validity */
    list.alloc = aws_default_allocator();
    list.item_size = item_size;

    /* Set up current_size and length */
    size_t num_items;
    __CPROVER_assume(num_items <= MAX_INITIAL_ITEM_ALLOCATION);
    list.current_size = num_items * item_size;
    list.length = num_items;

    /* Allocate data buffer */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Create a value to set */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Nondeterministic index */
    size_t index;
    /* Bound index to keep verification tractable */
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + 2);

    /* Save pre-call state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;

    int result = aws_array_list_set_at(&list, val, index);

    /* Postcondition 1: Return value is either AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Postcondition 2: item_size never changes */
    assert(list.item_size == old_item_size);

    /* Postcondition 3: If success, the list is still valid */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));

        /* Postcondition 4: If success, length must be at least index + 1 */
        assert(list.length >= index + 1);

        /* Postcondition 5: length consistency */
        if (index < old_length) {
            assert(list.length == old_length);
        } else {
            assert(list.length == index + 1);
        }

        /* Postcondition 6: current_size must be large enough to hold all elements */
        assert(list.current_size >= list.length * list.item_size);
    }

    /* Postcondition 7: If failure, length and current_size are unchanged */
    if (result == AWS_OP_ERR) {
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
    }

    /* Postcondition 8: list is still valid after the call regardless of result */
    assert(aws_array_list_is_valid(&list));
}
