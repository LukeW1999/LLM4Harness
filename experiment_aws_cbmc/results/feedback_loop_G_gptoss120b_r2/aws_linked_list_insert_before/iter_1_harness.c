#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose the node to insert before (use the tail sentinel, which is always valid) */
    struct aws_linked_list_node *before = &list.tail;

    /* 3. Allocate a node to add and assume it is not linked */
    struct aws_linked_list_node to_add;
    to_add.next = NULL;
    to_add.prev = NULL;
    __CPROVER_assume(to_add.next == NULL && to_add.prev == NULL);

    /* 4. Save old state needed for post‑condition checks */
    struct aws_linked_list_node *old_prev = before->prev;          /* node that will become predecessor of to_add */
    struct aws_linked_list_node *old_head_next = list.head.next;  /* original first real node (or tail if empty) */

    /* 5. Call the function under test */
    aws_linked_list_insert_before(before, &to_add);

    /* 6. Post‑condition: linkage of the newly inserted node */
    assert(to_add.next == before);
    assert(to_add.prev == old_prev);
    assert(before->prev == &to_add);
    assert(old_prev->next == &to_add);

    /* 7. Post‑condition: list sentinel updates */
    assert(list.tail.prev == &to_add);
    if (old_prev == &list.head) {
        /* insertion at the front of the list */
        assert(list.head.next == &to_add);
    } else {
        /* insertion not at the front – first element unchanged */
        assert(list.head.next == old_head_next);
    }

    /* 8. The newly added node must now be considered part of a list */
    assert(aws_linked_list_node_is_in_list(&to_add));

    /* 9. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
