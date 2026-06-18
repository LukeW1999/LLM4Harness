#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create at least one node in the list so that 'before' is not the head sentinel */
    struct aws_linked_list_node *existing = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(existing != NULL);
    aws_linked_list_node_reset(existing);
    aws_linked_list_push_back(&list, existing);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Choose the node that will serve as 'before' */
    struct aws_linked_list_node *before = existing;

    /* 4. Allocate a node that is not currently in any list */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 5. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next; /* should stay unchanged */

    /* 6. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 7. Post‑conditions that must hold after insertion */

    /* to_add is correctly linked */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(before->prev == to_add);
    if (old_before_prev != NULL) {
        assert(old_before_prev->next == to_add);
    }

    /* the node that followed 'before' is unchanged */
    assert(before->next == old_before_next);

    /* the inserted node is now considered part of a list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* list-wide validity invariant */
    assert(aws_linked_list_is_valid(&list));

    /* fields of the list structure that must remain unchanged */
    assert(&list.head == &old_list.head);
    assert(&list.tail == &old_list.tail);
    /* the sentinel connections head.prev ↔ tail and tail.next ↔ head stay the same */
    assert(list.head.prev == old_list.head.prev);
    assert(list.tail.next == old_list.tail.next);
    assert(list.head.prev == &list.tail);
    assert(list.tail.next == &list.head);
}
