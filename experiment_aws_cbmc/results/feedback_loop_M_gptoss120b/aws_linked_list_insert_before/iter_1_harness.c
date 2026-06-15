#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node that will already be in the list (optional) */
    struct aws_linked_list_node *existing = malloc(sizeof(*existing));
    __CPROVER_assume(existing != NULL);
    existing->next = NULL;
    existing->prev = NULL;
    __CPROVER_assume(!aws_linked_list_node_is_in_list(existing));
    aws_linked_list_push_back(&list, existing);
    __CPROVER_assume(aws_linked_list_node_is_in_list(existing));

    /* 3. Allocate the node to be inserted */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    to_add->next = NULL;
    to_add->prev = NULL;
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 4. Choose a valid 'before' node (either an existing node or the tail sentinel) */
    struct aws_linked_list_node *before;
    bool use_tail = nondet_bool();
    if (use_tail) {
        before = (struct aws_linked_list_node *)&list.tail;
    } else {
        __CPROVER_assume(!aws_linked_list_empty(&list));
        before = existing;
    }
    __CPROVER_assume(aws_linked_list_node_is_in_list(before));

    /* 5. Save old state needed for post‑condition checks */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node old_head = list.head;
    struct aws_linked_list_node old_tail = list.tail;

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 7. Post‑conditions that must hold after a successful insertion */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(before->prev == to_add);
    assert(old_before_prev->next == to_add);
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 8. Unchanged fields (sentinel nodes retain their unchanged links) */
    assert(list.head.prev == old_head.prev);
    assert(list.tail.next == old_tail.next);

    /* 9. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
