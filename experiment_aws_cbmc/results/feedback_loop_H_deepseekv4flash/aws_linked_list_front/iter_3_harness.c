#include <aws/common/linked_list.h>
#include <stdlib.h>

void aws_linked_list_front_harness() {
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);

    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    list->head.next = node;
    list->head.prev = &list->tail;
    node->next = &list->tail;
    node->prev = &list->head;
    list->tail.next = &list->head;
    list->tail.prev = node;

    __CPROVER_assume(aws_linked_list_is_valid_deep(list));
    __CPROVER_assume(!aws_linked_list_empty(list));

    struct aws_linked_list old_list = *list
