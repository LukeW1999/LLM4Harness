#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_pop_front_harness(void) {
    struct aws_array_list list;

    /* Use the default allocator for any internal allocations. */
    list.alloc = aws_default_allocator();

    /* Structural validity assumptions. */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Capture the pre‑state of the list. */
    size_t old_length   = aws_array_list_length(&list);
    size_t old_capacity = aws_array_list_capacity(&list);
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;

    /* Call the function under verification. */
    int ret = aws_array_list_pop_front(&list);

    /* Post‑condition: return value / error code correctness. */
    if (old_length > 0) {
        assert(ret == AWS_OP_SUCCESS);
        assert(aws_array_list_length(&list) == old_length - 1);
    } else {
        assert(ret != AWS_OP_SUCCESS);
        assert(ret == AWS_ERROR_LIST_EMPTY);
        assert(aws_array_list_length(&list) == old_length);
    }

    /* Post‑condition: capacity invariant. */
    assert(aws_array_list_capacity(&list) == old_capacity);

    /* Frame conditions: allocator, data pointer, and overall validity unchanged. */
    assert(list.alloc == old_alloc);
    assert(list.data == old_data);
    assert(aws_array_list_is_valid(&list));

    return 0;
}
