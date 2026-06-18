#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_remove_harness() {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate two nodes and put them into the list */
    struct aws_linked_list_node *node1 = malloc(sizeof(*node1));
    __CPROVER_assume(node1 != NULL);
    struct aws_linked_list_node *node2 = malloc(sizeof(*node2));
    __CPROVER_assume(node2 != NULL);

    /* Insert node1 first, then node2 – node2 will be the target of removal */
    aws_linked_list_push_back(&list, node1);
    aws_linked_list_push_back(&list, node2);

    /* 3. Save old state needed for post‑condition checks */
    struct aws_linked_list_node *old_prev = node2->prev;   /* should be node1 */
    struct aws_linked_list_node *old_next = node2->next;   /* should be &list.tail */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 4. Call the function under test */
    aws_linked_list_remove(node2);

    /* 5. Assert changed fields */
    /* node2 is reset */
    assert(node2->prev == NULL);
    assert(node2->next == NULL);

    /* surrounding nodes are linked together */
    assert(old_prev->next == old_next);
    assert(old_next->prev == old_prev);

    /* 6. Assert unchanged fields */
    /* list sentinels themselves must stay the same */
    assert(list.head.prev == &list.head);
    assert(list.tail.next == &list.tail);
    /* head and tail connections that are not affected by removal stay unchanged */
    assert(list.head.next == old_head_next);
    assert(list.tail.prev == old_tail_prev);

    /* 7. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
