#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound the list size for CBMC tractability */
#define MAX_INITIAL_ITEM_ALLOCATION 4
#define ITEM_SIZE 4

void aws_array_list_pop_front_harness(void) {
    /* Allocate and set up the list */
    struct aws_array_list list;

    /* Use a fixed item size for tractability */
    size_t item_size = ITEM_SIZE;

    /* Nondeterministic length bounded for CBMC */
    size_t length;
    __CPROVER_assume(length <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t current_size;
    __CPROVER_assume(current_size == length * item_size);

    /* Set up data buffer */
    void *data = NULL;
    if (current_size > 0) {
        data = malloc(current_size);
        __CPROVER_assume(data != NULL);
    }

    list.alloc = aws_default_allocator();
    list.current_size = current_size;
    list.length = length;
    list.item_size = item_size;
    list.data = data;

    /* Precondition: list must be valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save pre-call state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;

    /* Call the function under test */
    int result = aws_array_list_pop_front(&list);

    /* Postconditions: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Postconditions: return value and error code */
    if (old_length == 0) {
        /* Empty list: should return error */
        assert(result == AWS_OP_ERR);
        /* Length unchanged */
        assert(list.length == 0);
    } else {
        /* Non-empty list: should succeed */
        assert(result == AWS_OP_SUCCESS);
        /* Length decremented by 1 */
        assert(list.length == old_length - 1);
    }

    /* Postconditions: frame conditions */
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    /* data pointer should remain the same allocation */
    assert(list.data == old_data);
}
