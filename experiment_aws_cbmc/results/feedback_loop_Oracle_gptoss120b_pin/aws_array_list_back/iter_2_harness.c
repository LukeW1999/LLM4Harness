#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE               64

void aws_array_list_back_harness(void) {
    struct aws_array_list list;
    /* allocator required for a valid list */
    list.alloc = aws_default_allocator();

    /* nondet initialise the list fields (CBMC will assign them) */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.current_size >= list.item_size * list.length);
    __CPROVER_assume(list.length <= list.current_size / list.item_size);

    if (list.current_size > 0) {
        __CPROVER_assume(list.data != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(list.data, list.current_size));
    }

    /* allocate output buffer */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));
    }

    /* snapshot of the backing storage for frame condition */
    uint8_t *data_snapshot = NULL;
    if (list.current_size > 0) {
        data_snapshot = malloc(list.current_size);
        __CPROVER_assume(data_snapshot != NULL);
        memcpy(data_snapshot, list.data, list.current_size);
    }

    /* call the function under test */
    int ret = aws_array_list_back(&list, val);

    /* post‑conditions */
    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid after call");

    if (list.length > 0) {
        __CPROVER_assert(ret == AWS_OP_SUCCESS, "return success when list non‑empty");
        __CPROVER_assert(
            memcmp(val,
                   (uint8_t *)list.data + list.item_size * (list.length - 1),
                   list.item_size) == 0,
            "val receives the last element");
    } else {
        __CPROVER_assert(ret != AWS_OP_SUCCESS, "return not success when list empty");
        __CPROVER_assert(ret == AWS_ERROR_LIST_EMPTY, "error code is LIST_EMPTY");
    }

    /* frame condition: backing storage unchanged */
    if (list.current_size > 0) {
        __CPROVER_assert(
            memcmp(data_snapshot, list.data, list.current_size) == 0,
            "list data unchanged");
    }

    return;
}
