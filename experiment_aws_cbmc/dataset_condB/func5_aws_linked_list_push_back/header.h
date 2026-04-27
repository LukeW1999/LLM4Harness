/* From: include/aws/common/linked_list.h and linked_list.inl */

struct aws_linked_list_node {
    struct aws_linked_list_node *next;
    struct aws_linked_list_node *prev;
};

struct aws_linked_list {
    struct aws_linked_list_node head;  /* sentinel head node */
    struct aws_linked_list_node tail;  /* sentinel tail node */
};


AWS_STATIC_IMPL void aws_linked_list_push_back(
    struct aws_linked_list *list,
    struct aws_linked_list_node *node);

/* Validity predicates:
 * aws_linked_list_is_valid(list): circular head<->tail sentinel structure is intact,
 *   head.prev == NULL, tail.next == NULL
 * aws_linked_list_node_next_is_valid(node): node->next->prev == node
 * aws_linked_list_node_prev_is_valid(node): node->prev->next == node
 */
