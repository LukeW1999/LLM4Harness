#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    __CPROVER_assume(&list != NULL);
    __CPROVER_assume(&node != NULL);

    /* Reset node so it is not part of any list */
    aws_linked_list_node_reset(&node);

    /* Initialize list to empty valid state */
    aws_linked_list_init(&list);

    struct aws_linked_list_node *old_head = list.head;

    /* Call the function under test */
    aws_linked_list_push_front(&list, &node);

    /* Postcondition: node is now the list head */
    assert(list.head == &node);
    assert(list.head->prev == NULL);

    /* node->next points to old head (which may be NULL) */
    assert(node.next == old_head);

    /* If list was non-empty, old head's prev must point to node */
    if (old_head != NULL) {
        assert(old_head->prev == &node);
    } else {
        /* List was empty, tail must be node */
        assert(list.tail == &node);
    }
}
