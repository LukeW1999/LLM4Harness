#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 4
#define MAX_ITEM_SIZE 8

void aws_array_list_init_dynamic_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation;
    size_t item_size;

    /* Ground-truth preconditions */
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* Postcondition 1: Return value correctness */
    /* If multiplication overflows, result must be AWS_OP_ERR */
    size_t allocation_size;
    bool overflow = aws_mul_size_checked(initial_item_allocation, item_size, &allocation_size);
    if (overflow) {
        assert(result == AWS_OP_ERR);
    }

    /* Postcondition 2: On success, verify structural invariants */
    if (result == AWS_OP_SUCCESS) {
        /* item_size must be set correctly */
        assert(list.item_size == item_size);

        /* alloc must be set correctly */
        assert(list.alloc == alloc);

        /* length must be 0 on initialization */
        assert(list.length == 0);

        /* current_size must equal allocation_size */
        assert(list.current_size == allocation_size);

        /* If allocation_size > 0, data must be non-NULL */
        if (allocation_size > 0) {
            assert(list.data != NULL);
        }

        /* If allocation_size == 0, data must be NULL */
        if (allocation_size == 0) {
            assert(list.data == NULL);
        }

        /* Frame condition: current_size is 0 or data is non-NULL */
        assert(list.current_size == 0 || list.data != NULL);

        /* Validity predicate */
        assert(aws_array_list_is_valid(&list));

        /* Capacity invariant: capacity matches current_size / item_size */
        assert(aws_array_list_capacity(&list) == initial_item_allocation);

        /* Length invariant: length is 0 */
        assert(aws_array_list_length(&list) == 0);

        /* Clean up to avoid memory leaks */
        aws_array_list_clean_up(&list);
    } else {
        /* On error, result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }
}
