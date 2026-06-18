#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Select 'after' node from the list, not the tail sentinel */
    struct aws_linked_list_node *after = &list.head;
    size_t count = 0;
    while (after != &list.tail && count < MAX_LINKED_LIST_ITEM_ALLOCATION) {
        if (nondet_bool()) break;
        after = after->next;
        count++;
    }
    /* after is now some node in the list (could be head, but not tail) */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(after));

    /* 3. Create 'to_add' node, detached */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    to_add->next = NULL;
    to_add->prev = NULL;
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* 4. Save old state of all nodes in the list */
    struct aws_linked_list_node *old_next[MAX_LINKED_LIST_ITEM_ALLOCATION + 2];
    struct aws_linked_list_node *old_prev[MAX_LINKED_LIST_ITEM_ALLOCATION + 2];
    struct aws_linked_list_node *nodes[MAX_LINKED_LIST_ITEM_ALLOCATION + 2];
    size_t node_count = 0;

    /* Traverse real nodes */
    struct aws_linked_list_node *cur = list.head.next;
    while (cur != &list.tail && node_count < MAX_LINKED_LIST_ITEM_ALLOCATION) {
        nodes[node_count] = cur;
        old_next[node_count] = cur->next;
        old_prev[node_count] = cur->prev;
        node_count++;
        cur = cur->next;
    }
    /* Add sentinels */
    nodes[node_count] = &list.head;
    old_next[node_count] = list.head.next;
    old_prev[node_count] = list.head.prev;
    node_count++;
    nodes[node_count] = &list.tail;
    old_next[node_count] = list.tail.next;
    old_prev[node_count] = list.tail.prev;
    node_count++;

    /* Save specific old values for after */
    struct aws_linked_list_node *old_after_next = after->next;
    struct aws_linked_list_node *old_after_prev = after->prev;

    /* 5. Call function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Assert postconditions */

    /* Immediate linkage */
    assert(to_add->prev == after);
    assert(to_add->next == old_after_next);
    assert(after->next == to_add);
    assert(old_after_next->prev == to_add);
    assert(after->prev == old_after_prev);

    /* to_add is now in the list */
    assert(aws_linked_list_node_is_in_list(to_add));
    /* after is still in the list */
    assert(aws_linked_list_node_is_in_list(after));

    /* List validity */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    /* Check that other nodes' pointers are unchanged */
    for (size_t i = 0; i < node_count; i++) {
        struct aws_linked_list_node *n = nodes[i];
        if (n == after) {
            /* after's next changed to to_add, prev unchanged */
            assert(n->next == to_add);
            assert(n->prev == old_prev[i]);
        } else if (n == old_after_next) {
            /* old_after_next's prev changed to to_add, next unchanged */
            assert(n->prev == to_add);
            assert(n->next == old_next[i]);
        } else if (n == to_add) {
            /* to_add is new, not in old list, so skip */
        } else {
            /* unchanged */
            assert(n->next == old_next[i]);
            assert(n->prev == old_prev[i]);
        }
    }
}
