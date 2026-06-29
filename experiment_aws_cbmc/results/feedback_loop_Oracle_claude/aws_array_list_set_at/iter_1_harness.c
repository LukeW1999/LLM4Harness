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
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.data != NULL);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    void *val = malloc(list.item_size);
    __CPROVER_assume(val && AWS_MEM_IS_READABLE(val, list.item_size));

    size_t index;

    /* Save pre-call state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;

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

        /* Postcondition 5: If index was already within bounds, length is unchanged or increased */
        if (index < old_length) {
            assert(list.length == old_length);
        } else {
            /* index >= old_length, so length should be index + 1 */
            assert(list.length == index + 1);
        }

        /* Postcondition 6: current_size must be large enough to hold all elements */
        assert(list.current_size >= list.length * list.item_size);

        /* Postcondition 7: capacity must be at least length */
        assert(aws_array_list_capacity(&list) >= list.length);
    }

    /* Postcondition 8: If failure, length and current_size are unchanged */
    if (result == AWS_OP_ERR) {
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
    }

    /* Postcondition 9: list is still valid after the call regardless of result */
    assert(aws_array_list_is_valid(&list));
}
