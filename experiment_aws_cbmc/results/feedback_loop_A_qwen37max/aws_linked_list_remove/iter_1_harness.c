#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_remove_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    
    /* Ensure the list is not empty so we have a valid internal node to remove */
    __CPROVER_assume(!aws_linked_list_empty(&list));
    
    struct aws_linked_list_node *node = list.head.next;
    
    /* Save pointers to the adjacent nodes before removal */
    struct aws_linked_list_node *prev_node = node->prev;
    struct aws_linked_list_node *next_node = node->next;
    
    /* Call the function under test */
    aws_linked_list_remove(node);
    
    /* Assert postconditions: adjacent nodes now point to each other */
    assert(prev_node->next == next_node);
    assert(next_node->prev == prev_node);
    
    /* Assert postconditions: removed node is reset */
    assert(node->next == NULL);
    assert(node->prev == NULL);
    
    /* Assert validity invariant: the list remains valid after removal */
    assert(aws_linked_list_is_valid(&list));
}
