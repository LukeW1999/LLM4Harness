#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_before_harness() {
    /* Initialize a valid doubly linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* Allocate the node to insert, ensure it is not already linked */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    to_add->next = NULL;
    to_add->prev = NULL;

    /* Pick a node before which to insert.  Using list.head.next works even
     * if the list is empty (before will be the tail sentinel). */
    struct aws_linked_list_node *before = list.head.next;

    /* Save the state that should be preserved or change in predictable ways */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_head_next    = list.head.next;
    struct aws_linked_list_node *old_tail_prev    = list.tail.prev;
    struct aws_linked_list_node *old_head_prev    = list.head.prev;
    struct aws_linked_list_node *old_tail_next    = list.tail.next;

    /* Perform the insertion */
    aws_linked_list_insert_before(before, to_add);

    /* Direct linkage results */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(old_before_prev->next == to_add);
    assert(before->prev == to_add);

    /* Overall list invariants must still hold */
    assert(aws_linked_list_is_valid_deep(&list));

    /* Sentinel tail pointer changes if before was the tail sentinel */
    if (before == &list.tail) {
        assert(list.tail.prev == to_add);
    } else {
        assert(list.tail.prev == old_tail_prev);
    }

    /* Sentinel head pointer changes if the previous node was the head sentinel */
    if (old_before_prev == &list.head) {
        assert(list.head.next == to_add);
    } else {
        assert(list.head.next == old_head_next);
    }

    /* The other sentinel fields must not change */
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);

    /* Evidence that to_add now belongs to a bidirectional chain */
    assert(to_add->prev != NULL);
    assert(to_add->next != NULL);
    assert(to_add->prev->next == to_add);
    assert(to_add->next->prev == to_add);
}
