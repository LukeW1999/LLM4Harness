#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 4
#define MAX_ITEM_SIZE 8

void aws_array_list_pop_front_harness(void) {
    struct aws_array_list list;

    /* Nondeterministic initialization */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Use static or dynamic list nondeterministically */
    bool use_static;
    if (use_static) {
        /* Static list */
        size_t buf_size;
        __CPROVER_assume(buf_size == initial_item_allocation * item_size);
        void *raw_array = malloc(buf_size > 0 ? buf_size : 1);
        __CPROVER_assume(raw_array != NULL || buf_size == 0);
        aws_array_list_init_static(&list, raw_array, initial_item_allocation, item_size);
        /* Set length nondeterministically within capacity */
        size_t len;
        __CPROVER_assume(len <= initial_item_allocation);
        list.length = len;
    } else {
        /* Dynamic list */
        if (initial_item_allocation > 0) {
            int rc = aws_array_list_init_dynamic(
                &list, aws_default_allocator(), initial_item_allocation, item_size);
            __CPROVER_assume(rc == AWS_OP_SUCCESS);
        } else {
            /* Zero allocation dynamic list */
            list.alloc = aws_default_allocator();
            list.current_size = 0;
            list.length = 0;
            list.item_size = item_size;
            list.data = NULL;
        }
        /* Set length nondeterministically within capacity */
        size_t capacity = (list.item_size > 0) ? (list.current_size / list.item_size) : 0;
        size_t len;
        __CPROVER_assume(len <= capacity);
        list.length = len;
    }

    /* Ground-truth preconditions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save pre-call state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    int result = aws_array_list_pop_front(&list);

    /* Postcondition 1: Return value correctness */
    if (old_length > 0) {
        /* Should succeed */
        assert(result == AWS_OP_SUCCESS);
    } else {
        /* Should fail with LIST_EMPTY error */
        assert(result == AWS_OP_ERR);
        assert(aws_last_error() == AWS_ERROR_LIST_EMPTY);
    }

    /* Postcondition 2: Length invariants */
    if (old_length > 0) {
        /* Length should decrease by 1 */
        assert(list.length == old_length - 1);
    } else {
        /* Length should remain 0 */
        assert(list.length == old_length);
        assert(list.length == 0);
    }

    /* Postcondition 3: item_size is unchanged */
    assert(list.item_size == old_item_size);

    /* Postcondition 4: current_size does not decrease (pop_front doesn't shrink) */
    assert(list.current_size >= list.length * list.item_size);

    /* Postcondition 5: The list remains valid after the operation */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition 6: Allocator is unchanged */
    assert(list.alloc == old_alloc);

    /* Postcondition 7: current_size is unchanged (pop_front doesn't reallocate) */
    assert(list.current_size == old_current_size);
}
