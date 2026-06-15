#include <aws/common/linked_list.h>
#include <assert.h>

void aws_linked_list_insert_after_harness(void) {
    /* Initialise an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Allocate three distinct nodes */
    struct aws_linked_list_node after_node;
    struct aws_linked_list_node dummy_node;
    struct aws_linked_list_node to_add_node;
    __CPROVER_assume(&after_node != &dummy_node);
    __CPROVER_assume(&after_node != &to_add_node);
    __CPROVER_assume(&dummy_node != &to_add_node);

    /* Ensure they start unlinked */
    aws_linked_list_node_reset(&after_node);
    aws_linked_list_node_reset(&dummy_node);
    aws_linked_list_node_reset(&to_add_node);
    __CPROVER_assume(after_node.next == NULL);
    __CPROVER_assume(after_node.prev == NULL);
    __CPROVER_assume(dummy_node.next == NULL);
    __CPROVER_assume(dummy_node.prev == NULL);
    __CPROVER_assume(to_add_node.next == NULL);
    __CPROVER_assume(to_add_node.prev == NULL);

    /* Build a list with two elements: after_node followed by dummy_node */
    aws_linked_list_push_back(&list, &after_node);
    aws_linked_list_push_back(&list, &dummy_node);

    /* after_node must have a valid non‑sentinel successor */
    __CPROVER_assume(after_node.next == &dummy_node);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(&after_node));

    /* Save old state for later comparison */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_after_next = after_node.next;

    /* Call the function under test */
    aws_linked_list_insert_after(&after_node, &to_add_node);

    /* Post‑conditions */
    assert(after_node.next == &to_add_node);               /* after_node now points to new node */
    assert(to_add_node.prev == &after_node);               /* new node points back to after_node */
    assert(to_add_node.next == old_after_next);            /* new node links to former successor */
    assert(old_after_next->prev == &to_add_node);          /* former successor now points back to new node */

    /* Unchanged list sentinel links */
    assert(list.head.next == old_list.head.next);          /* head.next unchanged (still after_node) */
    assert(list.tail.prev == old_list.tail.prev);          /* tail.prev unchanged (still dummy_node) */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    /* after_node's prev unchanged (still points to head sentinel) */
    assert(after_node.prev == &list.head);

    /* dummy_node's links: prev should now be to_add_node, next unchanged (tail sentinel) */
    assert(dummy_node.prev == &to_add_node);
    assert(dummy_node.next == &list.tail);

    /* to_add_node was previously unlinked */
    assert(to_add_node.prev != NULL);
    assert(to_add_node.next != NULL);

    /* List validity invariant must still hold */
    assert(aws_linked_list_is_valid(&list));
}
