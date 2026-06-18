#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Allocate and initialize a list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create a node that will be in the list (so the list is non‑empty) */
    struct aws_linked_list_node existing;
    aws_linked_list_node_reset(&existing);
    aws_linked_list_push_back(&list, &existing);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. 'before' must point to a node that is already in the list */
    struct aws_linked_list_node *before = &existing;
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(aws_linked_list_node_is_in_list(before));

    /* 4. Create a node that is not yet in any list */
    struct aws_linked_list_node to_add;
    aws_linked_list_node_reset(&to_add);
    __CPROVER_assume(&to_add != before);
    __CPROVER_assume(to_add.next == NULL && to_add.prev == NULL);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(&to_add));

    /* 5. Save old state needed for post‑condition checks */
    struct aws_linked_list_node *old_prev = before->prev;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, &to_add);

    /* 7. Post‑conditions: the new node is inserted immediately before 'before' */
    assert(to_add.next == before);
    assert(to_add.prev == old_prev);
    assert(before->prev == &to_add);
    assert(old_prev->next == &to_add);

    /* 8. List tail sentinel link that must remain unchanged */
    assert(list.tail.next == &list.tail);
    assert(list.tail.prev == old_tail_prev);

    /* 9. If 'before' was the first real element, head.next now points to the new node */
    assert(list.head.next == &to_add);

    /* 10. The original node's links are updated correctly */
    assert(existing.prev == &to_add);
    assert(existing.next == &list.tail);

    /* 11. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* 12. Membership predicates */
    assert(aws_linked_list_node_is_in_list(&to_add));
    assert(aws_linked_list_node_is_in_list(before));
}
