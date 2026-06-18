#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_next_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_linked_list old = list;

    /* 3. Choose a node that is part of the list (head or interior, but not tail) */
    const struct aws_linked_list_node *node;
    bool pick_interior = nondet_bool();
    __CPROVER_assume(pick_interior == true || pick_interior == false);

    if (pick_interior) {
        /* walk a nondeterministic number of steps from the first element */
        const struct aws_linked_list_node *cur = list.head.next;
        size_t steps = nondet_size_t();
        __CPROVER_assume(steps <= MAX_LINKED_LIST_ITEM_ALLOCATION);
        while (steps > 0 && cur != &list.tail) {
            cur = cur->next;
            steps--;
        }
        node = cur;
    } else {
        /* pick the head (tail is excluded because its next pointer is NULL) */
        node = &list.head;
    }

    __CPROVER_assume(node != NULL);
    __CPROVER_assume(node != &list.tail);               /* precondition of aws_linked_list_next */
    __CPROVER_assume(node->next != NULL);               /* next must be a valid pointer */
    __CPROVER_assume(node->next->prev == node);         /* node_next_is_valid condition */

    /* 4. Save the expected result */
    struct aws_linked_list_node *expected = node->next;

    /* 5. Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 6. Post‑condition: result must be the next pointer of the input node */
    assert(result == expected);

    /* 7. Unchanged fields: the list must remain exactly the same */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 8. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
