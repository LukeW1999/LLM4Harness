#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Allocate and bound the list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create a node that will be part of the list (the "before" node) */
    struct aws_linked_list_node *existing = malloc(sizeof(*existing));
    __CPROVER_assume(existing != NULL);
    aws_linked_list_node_reset(existing);
    aws_linked_list_push_back(&list, existing);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Choose the node that we will insert before */
    struct aws_linked_list_node *before = existing;

    /* 4. Allocate the node to be inserted and ensure it is not in any list */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(to_add->next == NULL && to_add->prev == NULL);

    /* 5. Save old state for post‑condition checks */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;
    struct aws_linked_list_node *old_head_prev   = list.head.prev;
    struct aws_linked_list_node *old_tail_next   = list.tail.next;
    struct aws_linked_list_node *old_tail_prev   = list.tail.prev;

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 7. Post‑conditions that must hold after a successful insertion */

    /* to_add is linked correctly */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);

    /* before's prev pointer is updated */
    assert(before->prev == to_add);

    /* the node that previously preceded 'before' now points forward to to_add */
    if (old_before_prev != NULL) {
        assert(old_before_prev->next == to_add);
    }

    /* the node that follows 'before' is unchanged */
    assert(before->next == old_before_next);

    /* list sentinel connections that are not supposed to change */
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);

    /* if 'before' was not the last element, tail.prev stays the same;
       otherwise it should now point to the newly inserted node */
    if (old_before_next == &list.tail) {
        assert(list.tail.prev == to_add);
    } else {
        assert(list.tail.prev == old_tail_prev);
    }

    /* the inserted node is now considered to be in a list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 8. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
