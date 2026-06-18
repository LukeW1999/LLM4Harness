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

    /* 2. Create an existing node and add it to the list so we have a valid 'before' node */
    struct aws_linked_list_node *existing = malloc(sizeof(*existing));
    __CPROVER_assume(existing != NULL);
    aws_linked_list_node_reset(existing);
    aws_linked_list_push_back(&list, existing);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Allocate the node to be inserted */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    /* Assume the node to add is not currently in any list */
    __CPROVER_assume(to_add->next == NULL);
    __CPROVER_assume(to_add->prev == NULL);

    /* 4. Choose the 'before' node (must be a node that is already in the list) */
    struct aws_linked_list_node *before = existing;
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(aws_linked_list_node_next_is_valid(before));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(before));

    /* Save the node that currently precedes 'before' */
    struct aws_linked_list_node *old_prev = before->prev;

    /* 5. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 6. Post‑condition: linkage of the newly inserted node */
    assert(to_add->next == before);
    assert(to_add->prev == old_prev);
    assert(before->prev == to_add);
    assert(old_prev->next == to_add);

    /* 7. Unchanged fields of the list structure (sentinel nodes) */
    /* The head and tail sentinel nodes themselves should remain the same objects */
    assert(&list.head != NULL);
    assert(&list.tail != NULL);
    /* The tail's prev should still be the original last element (which is 'before' in this setup) */
    assert(list.tail.prev == before);
    /* The head's next should now be the newly inserted node */
    assert(list.head.next == to_add);

    /* 8. Validity invariants must hold after insertion */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));
}
