#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_rend_harness(void) {
    struct aws_linked_list list;

    /* Assume the list is structurally valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save a copy of the list to check frame conditions later */
    struct aws_linked_list old_list = list;

    /* Call the function under verification */
    const struct aws_linked_list_node *r = aws_linked_list_rend(&list);

    /* 1. Return value correctness */
    __CPROVER_assert(r == &list.head,
                     "aws_linked_list_rend should return pointer to the list head");

    /* 2. List remains valid after the call */
    __CPROVER_assert(aws_linked_list_is_valid(&list),
                     "list must remain valid after aws_linked_list_rend");

    /* 3. Frame condition: the list memory is unchanged */
    __CPROVER_assert(memcmp(&old_list, &list, sizeof(list)) == 0,
                     "aws_linked_list_rend must not modify the list");

    return 0;
}
