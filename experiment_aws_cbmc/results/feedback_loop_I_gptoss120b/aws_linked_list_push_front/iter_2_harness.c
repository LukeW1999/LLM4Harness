#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness(void) {
    /* Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(list.head.prev == NULL);
    __CPROVER_assume(list.tail.next == NULL);

    /* Decide whether the list is initially empty */
    bool list_nonempty = nondet_bool();

    struct aws_linked_list_node *old_front = NULL;

    if (list_nonempty) {
        /* Create an existing node and link it between head and tail */
        old_front = malloc(sizeof(*old_front));
        __CPROVER_assume(old_front != NULL);
        old_front->prev = &list.head;
        old_front->next = &list.tail;
        list.head.next = old_front;
        list.tail.prev = old_front;
    } else {
        /* Empty list: head.next points to tail, tail.prev points to head */
        list.head.next = &list.tail;
        list.tail.prev = &list.head;
    }

    /* Allocate a new node to push to the front */
    struct aws_linked_list_node *new_node = malloc(sizeof(*new_node));
    __CPROVER_assume(new_node != NULL);
    /* The node's fields are nondeterministic before the call */
    new_node->prev = NULL;
    new_node->next = NULL;

    /* Save old state for later assertions */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* Call the function under test */
    aws_linked_list_push_front(&list, new_node);

    /* Post‑condition: list must remain valid */
    assert(aws_linked_list_is_valid(&list));

    /* Post‑condition: new_node is now the first element */
    assert(list.head.next == new_node);
    assert(new_node->prev == &list.head);
    if (list_nonempty) {
        assert(new_node->next == old_front);
        /* Tail unchanged */
        assert(list.tail.prev == old_tail_prev);
    } else {
        assert(new_node->next == &list.tail);
        /* Tail now points to new_node */
        assert(list.tail.prev == new_node);
    }

    /* Unchanged sentinel fields */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
