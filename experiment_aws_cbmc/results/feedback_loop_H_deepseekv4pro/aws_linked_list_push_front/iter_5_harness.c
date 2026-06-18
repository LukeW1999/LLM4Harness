#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;
    struct aws_linked_list *list_ptr = &list;
    struct aws_linked_list_node *node_ptr = &node;

    __CPROVER_assume(list_ptr != NULL);
    __CPROVER_assume(node_ptr != NULL);

    /* Reset node so it is not part of any list */
    aws_linked_list_node_reset(node_ptr);

    /* Initialize list to empty valid state */
    aws_linked_list_init(list_ptr);

    struct aws_linked_list_node *old_head = list.head;

    /* Call the function under test */
    aws_linked_list_push_front(list_ptr, node_ptr);

    /* Postcondition: node is now the list head */
    assert(list.head == node_ptr);
    assert(list.head->prev == NULL);

    /* node->next points to old head (which may be NULL) */
    assert(node_ptr->next == old_head);

    /* If list was non-empty, old head's prev must point to node */
    if (old_head != NULL) {
        assert(old_head->prev == node_ptr);
    } else {
        /* List was empty, tail must be node */
        assert(list.tail == node_ptr);
    }
}
