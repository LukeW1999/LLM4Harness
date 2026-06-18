#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    /* 1. Set up a linked list with some nodes allocated */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Create the node to insert before (must be a valid node in the list) */
    /* We use the list's tail as the "before" node (insert before tail = push_back) */
    /* Or we can use a non-deterministic node that is already in the list */
    /* For generality, use list.tail as "before" since it's always valid */
    struct aws_linked_list_node *before = &list.tail;

    /* 3. Create the node to add */
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    /* 4. Save old state */
    struct aws_linked_list_node *old_before_prev = before->prev;

    /* 5. Call function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 6. Assert postconditions */
    /* to_add->next should point to before */
    assert(to_add->next == before);

    /* to_add->prev should point to what was before->prev */
    assert(to_add->prev == old_before_prev);

    /* old_before_prev->next should now point to to_add */
    assert(old_before_prev->next == to_add);

    /* before->prev should now point to to_add */
    assert(before->prev == to_add);

    /* The list should still be valid */
    assert(aws_linked_list_is_valid(&list));

    /* The list should not be empty (we just inserted a node) */
    assert(!aws_linked_list_empty(&list));

    /* Bidirectional linkage: to_add->next->prev == to_add */
    assert(to_add->next->prev == to_add);

    /* Bidirectional linkage: to_add->prev->next == to_add */
    assert(to_add->prev->next == to_add);
}
