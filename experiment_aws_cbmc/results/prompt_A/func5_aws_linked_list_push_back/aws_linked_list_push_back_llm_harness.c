#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Assuming AWS_STATIC_IMPL is defined as static inline for this context
#define AWS_STATIC_IMPL static inline

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

static inline int aws_linked_list_is_valid(const struct aws_linked_list *list) {
    return list->head.next == &list->tail &&
           list->tail.prev == &list->head &&
           list->head.prev == NULL &&
           list->tail.next == NULL;
}

static inline int aws_linked_list_node_next_is_valid(const struct aws_linked_list_node *node) {
    return node->next && node->next->prev == node;
}

static inline int aws_linked_list_node_prev_is_valid(const struct aws_linked_list_node *node) {
    return node->prev && node->prev->next == node;
}

void aws_linked_list_push_back_harness() {
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));

    // Initialize non-deterministically
    __CPROVER_assume(list != NULL);
    __CPROVER_assume(node != NULL);

    // Precondition: list is a valid aws_linked_list
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->head.prev = NULL;
    list->tail.next = NULL;

    // Precondition: node != NULL
    // Node's next and prev are non-deterministic but should be valid after insertion
    node->next = NULL;
    node->prev = NULL;

    // Save old state for postconditions
    struct aws_linked_list_node *old_tail_prev = list->tail.prev;

    // Call the function under verification
    aws_linked_list_push_back(list, node);

    // Postcondition: list remains valid
    assert(aws_linked_list_is_valid(list));

    // Postcondition: list->tail.prev == node
    assert(list->tail.prev == node);

    // Postcondition: node->prev == old list->tail.prev
    assert(node->prev == old_tail_prev);

    // Postcondition: aws_linked_list_node_prev_is_valid(node)
    assert(aws_linked_list_node_prev_is_valid(node));

    // Postcondition: aws_linked_list_node_next_is_valid(node)
    assert(aws_linked_list_node_next_is_valid(node));

    free(list);
    free(node);
}
