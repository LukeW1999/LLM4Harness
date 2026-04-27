/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_clean_up
 *
 * The function:
 * - If list->alloc and list->data are both non-null, releases list->data via aws_mem_release
 * - Then zeros out the entire list struct (AWS_ZERO_STRUCT(*list))
 *
 * Postconditions:
 * - list->alloc == NULL
 * - list->data == NULL
 * - list->current_size == 0
 * - list->length == 0
 * - list->item_size == 0
 * - aws_array_list_is_valid(&list) holds (a zeroed list is valid)
 */
void aws_array_list_clean_up_harness() {
    /* data structure */
    struct aws_array_list list;

    /* assumptions: bound the list to keep state space manageable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* perform operation under verification */
    aws_array_list_clean_up(&list);

    /* assertions: after clean_up, the struct is zeroed */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* validity invariant: a zeroed list is valid */
    assert(aws_array_list_is_valid(&list));
}
