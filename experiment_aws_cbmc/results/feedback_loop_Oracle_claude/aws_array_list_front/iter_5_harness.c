#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_array_list_front_harness(void) {
    /* Declare and non-deterministically initialize the array list */
    struct aws_array_list list;

    /* Non-deterministically choose item_size (must be > 0 and bounded) */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= 64);

    /* Non-deterministically choose length and capacity */
    size_t length;
    size_t capacity;
    __CPROVER_assume(capacity >= length);
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(capacity <= 8);
    __CPROVER_assume(length <= capacity);

    list.item_size = item_size;
    list.length = length;

    /* Avoid overflow */
    __CPROVER_assume(capacity <= SIZE_MAX / item_size);
    size_t current_size = capacity * item_size;
    list.current_size = current_size;

    /* Set up data pointer - always allocate since capacity > 0 */
    list.data = malloc(current_size);
    __CPROVER_assume(list.data != NULL);

    /* Set allocator */
    list.alloc = aws_default_allocator();

    /* Ensure the list is valid before calling the function */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate output buffer of item_size bytes */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Save state before call */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    size_t old_item_size = list.item_size;

    /* Call the function under test */
    int result = aws_array_list_front(&list, val);

    /* Postcondition 1: Return value correctness */
    if (old_length > 0) {
        /* Should succeed */
        assert(result == AWS_OP_SUCCESS);
    } else {
        /* Should fail with AWS_ERROR_LIST_EMPTY */
        assert(result == AWS_OP_ERR);
    }

    /* Postcondition 2: List length invariant - length must not change */
    assert(list.length == old_length);

    /* Postcondition 3: List capacity invariant - current_size must not change */
    assert(list.current_size == old_current_size);

    /* Postcondition 4: List item_size must not change */
    assert(list.item_size == old_item_size);

    /* Postcondition 5: List data pointer must not change */
    assert(list.data == old_data);

    /* Postcondition 6: List must still be valid after the call */
    assert(aws_array_list_is_valid(&list));
}
