#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_prev_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *node;
    __CPROVER_assume(AWS_MEM_IS_READABLE(node, sizeof(struct aws_linked_list_node)));

    struct aws_linked_list_node *rval = aws_linked_list_prev(node);
    
    assert(rval == node->prev);
    assert(aws_linked_list_is_valid(&list));
}
