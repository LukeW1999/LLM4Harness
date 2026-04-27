#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/pointer_utils.h>

void aws_linked_list_push_back_harness() {
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));

    // Non-deterministic initialization
    if (nondet_bool()) {
        list->head.next = &list->tail;
        list->head.prev = NULL;
        list->tail.prev = &list->head;
        list->tail.next = NULL;
    } else {
        list->head.next = nondet_pointer();
        list->head.prev = nondet_pointer();
        list->tail.prev = nondet_pointer();
        list->tail.next = nondet_pointer();
    }

    node->next = nondet_pointer();
    node->prev = nondet_pointer();

    // Preconditions
    __CPROVER_assume(list != NULL);
    __CPROVER_assume(node != NULL);
    __CPROVER_assume(list->head.next == &list->tail);
    __CPROVER_assume(list->head.prev == NULL);
    __CPROVER_assume(list->tail.prev == &list->head);
    __CPROVER_assume(list->tail.next == NULL);

    // Save old state for postconditions
    struct aws_linked_list_node *old_tail_prev = list->tail.prev;

    // Call the function under test
    aws_linked_list_push_back(list, node);

    // Postconditions
    assert(list->head.next == &list->tail);
    assert(list->head.prev == NULL);
    assert(list->tail.prev == node);
    assert(list->tail.next == NULL);
    assert(node->next == &list->tail);
    assert(node->prev == old_tail_prev);
    assert(old_tail_prev->next == node);
}
