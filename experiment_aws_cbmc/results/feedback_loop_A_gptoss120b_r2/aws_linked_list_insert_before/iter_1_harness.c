#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Allocate and initialize a list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 2. Create a node that will be in the list */
    struct aws_linked_list_node *existing = malloc(sizeof(*existing));
    __CPROVER_assume(existing != NULL);
    aws_linked_list_node_reset(existing);
    aws_linked_list_push_back(&list, existing);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(&list));

    /* 3. Create a node that will be inserted */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 4. Choose the node before which we will insert */
    struct aws_linked_list_node *before = existing;

    /* 5. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 7. Post‑condition assertions */

    /* Changed fields */
    assert(list.head.next == to_add);                     /* new first element */
    assert(to_add->prev == old_before_prev);              /* links back to former predecessor (head) */
    assert(to_add->next == before);                       /* points to the node we inserted before */
    assert(before->prev == to_add);                       /* predecessor of before is now to_add */
    assert(list.tail.prev == before);                     /* last element remains the original node */

    /* Unchanged fields */
    assert(before->next == old_before_next);              /* next pointer of before unchanged (still tail) */
    assert(list.head.prev == old_list.head.prev);         /* head sentinel unchanged */
    assert(list.tail.next == old_list.tail.next);         /* tail sentinel unchanged */
    assert(old_head_next == before);                      /* old head.next was the node we inserted before */
    assert(old_tail_prev == before);                      /* old tail.prev was the same node (single‑element list) */

    /* Validity invariants */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
    assert(aws_linked_list_node_is_in_list(to_add));
    assert(aws_linked_list_node_is_in_list(before));
}
