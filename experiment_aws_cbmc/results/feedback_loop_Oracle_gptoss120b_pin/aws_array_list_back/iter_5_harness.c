#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "aws/common/common.h"
#include "aws/common/array_list.h"
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               32U

void aws_array_list_back_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Initialize list with bounded size and item size */
    make_aws_array_list(&list, alloc, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);

    /* Constrain to a non‑empty list so that back() succeeds */
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.data != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(list.data, list.current_size));

    /* Bound the current size to avoid huge allocations */
    __CPROVER_assume(list.current_size <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);

    /* Allocate output buffer */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));

    /* Preconditions on the list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Snapshot of list state for frame condition checks */
    size_t old_length      = list.length;
    size_t old_current_sz  = list.current_size;
    size_t old_item_sz     = list.item_size;
    void *old_data = list.data; /* keep pointer for later readability check */

    /* Call the function under test */
    int ret = aws_array_list_back(&list, val);

    /* Post‑condition: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Return‑value correctness for non‑empty list */
    assert(ret == AWS_OP_SUCCESS);

    /* Frame conditions: list fields unchanged */
    assert(list.length == old_length);
    assert(list.current_size == old_current_sz);
    assert(list.item_size == old_item_sz);
    /* Data buffer should remain unchanged */
    assert(AWS_MEM_IS_READABLE(old_data, list.current_size));
    assert(memcmp(list.data, old_data, list.current_size) == 0);

    return;
}
