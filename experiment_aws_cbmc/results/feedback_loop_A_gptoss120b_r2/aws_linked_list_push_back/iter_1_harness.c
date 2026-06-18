#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_push_back_harness() {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node to push */
    struct aws_linked_list_node *node = malloc(sizeof(*node));
    __CPROVER_assume(node != NULL);

    /* 3. Save old state */
    struct aws_linked_list old = list;
    struct aws_linked_list_node *old_last = list.tail.prev;

    /* 4. Call the function under test */
    aws_linked_list_push_back(&list, node);

    /* 5. Post‑conditions that must hold on success */
    /* The new node becomes the last element */
    assert(list.tail.prev == node);
    /* The new node links back to the previous last element */
    assert(node->prev == old_last);
    /* The new node points forward to the tail sentinel */
    assert(node->next == &list.tail);
    /* The previous last element now points forward to the new node */
    assert(old_last->next == node);

    /* 6. Fields that must remain unchanged */
    /* Sentinel invariants */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    /* The head sentinel itself (except its next pointer which may change) */
    assert(list.head.prev == old.head.prev);
    /* The tail sentinel itself (except its prev pointer which changes) */
    assert(list.tail.next == old.tail.next);

    /* 7. Validity invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_node_next_is_valid(node));
    assert(aws_linked_list_node_prev_is_valid(node));
}
