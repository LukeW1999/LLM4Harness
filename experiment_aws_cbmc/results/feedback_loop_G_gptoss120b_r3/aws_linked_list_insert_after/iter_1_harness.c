#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create an initial node and insert it so the list is non‑empty */
    struct aws_linked_list_node *initial_node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(initial_node != NULL);
    aws_linked_list_node_reset(initial_node);
    aws_linked_list_push_back(&list, initial_node);

    /* 3. Choose a node that will serve as the “after” argument.
       It may be the initial node or the list head sentinel. */
    struct aws_linked_list_node *after = nondet_bool() ? initial_node : &list.head;

    /* 4. Allocate a node to be added */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 5. Save old state for later comparison */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_after = after;
    struct aws_linked_list_node *old_next = after->next;

    /* 6. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 7. Post‑condition: linkage updates */
    assert(after->next == to_add);                /* after now points to the new node */
    assert(to_add->prev == after);                /* new node’s prev points back to after */
    assert(to_add->next == old_next);             /* new node’s next is the former after->next */
    assert(old_next->prev == to_add);              /* former next node now points back to new node */

    /* 8. Unchanged fields of the list structure (sentinel connections) */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);

    /* 9. Invariant: the list remains valid */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* 10. The added node is now considered part of a list */
    assert(aws_linked_list_node_is_in_list(to_add));
}
