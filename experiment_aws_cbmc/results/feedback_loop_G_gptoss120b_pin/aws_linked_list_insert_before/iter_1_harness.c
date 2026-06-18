#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_before_harness(void) {
    /* Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Allocate two nodes */
    struct aws_linked_list_node *before = malloc(sizeof(*before));
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(to_add != NULL);

    /* Initialize the list with a single element 'before' */
    aws_linked_list_push_back(&list, before);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Ensure 'to_add' is not part of any list */
    to_add->next = NULL;
    to_add->prev = NULL;
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_before = *before;
    struct aws_linked_list_node old_to_add = *to_add;

    /* Preconditions for insertion */
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(before));

    /* Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* Postconditions: linkage around the inserted node */
    assert(to_add->next == before);
    assert(to_add->prev == old_before.prev);
    assert(before->prev == to_add);
    assert(old_before.prev->next == to_add);

    /* Head and tail sentinel connections */
    assert(list.head.next == to_add);
    assert(list.tail.prev == before);
    assert(list.head.prev == &list.tail);
    assert(list.tail.next == &list.head);

    /* Unchanged connections of existing nodes */
    assert(before->next == &list.tail);
    assert(before->next == old_before.next);
    assert(before->prev != old_before.prev); /* changed to to_add */
    assert(to_add->next != NULL && to_add->prev != NULL);

    /* Ensure the list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
