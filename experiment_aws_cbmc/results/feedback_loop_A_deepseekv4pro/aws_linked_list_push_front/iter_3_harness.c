#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_swap_nodes_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Non-deterministically pick two distinct nodes from the list */
    struct aws_linked_list_node *a = NULL;
    struct aws_linked_list_node *b = NULL;

    struct aws_linked_list_node *iter = list.head.next;
    while (iter != &list.tail) {
        if (__CPROVER_bool && a == NULL) {
            a = iter;
        } else if (__CPROVER_bool && b == NULL && iter != a) {
            b = iter;
        }
        iter = iter->next;
    }

    __CPROVER_assume(a != NULL && b != NULL && a != b);
    __CPROVER_assume(aws_linked_list_node_is_in_list(a));
    __CPROVER_assume(aws_linked_list_node_is_in_list(b));

    /* 3. Save old state of the two nodes and the list's sentinel connections */
    struct aws_linked_list_node old_a = *a;
    struct aws_linked_list_node old_b = *b;

    /* 4. Call function under test */
    aws_linked_list_swap_nodes(a, b);

    /* 5. Assert postconditions from implementation */
    assert(aws_linked_list_node_prev_is_valid(a));
    assert(aws_linked_list_node_next_is_valid(a));
    assert(aws_linked_list_node_prev_is_valid(b));
    assert(aws_linked_list_node_next_is_valid(b));

    /* 6. Assert list validity invariant holds */
    assert(aws_linked_list_is_valid(&list));

    /* 7. Assert nodes are still in the list */
    assert(aws_linked_list_node_is_in_list(a));
    assert(aws_linked_list_node_is_in_list(b));

    /* 8. Assert sentinel nodes remain unchanged */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);

    /* 9. Assert the nodes have swapped positions:
     *    a now occupies b's old place, b occupies a's old place */
    assert(a->prev == old_b.prev);
    assert(a->next == old_b.next);
    assert(b->prev == old_a.prev);
    assert(b->next == old_a.next);

    /* 10. Assert neighbors now point to the swapped nodes */
    if (old_a.prev != NULL) {
        assert(old_a.prev->next == b);
    }
    if (old_a.next != NULL) {
        assert(old_a.next->prev == b);
    }
    if (old_b.prev != NULL) {
        assert(old_b.prev->next == a);
    }
    if (old_b.next != NULL) {
        assert(old_b.next->prev == a);
    }
}
