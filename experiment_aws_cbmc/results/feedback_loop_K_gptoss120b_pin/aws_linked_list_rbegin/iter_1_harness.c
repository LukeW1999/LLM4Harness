/*=== Contract ===
Preconditions:
  - list != NULL
  - aws_linked_list_is_valid(list) == true
  - aws_linked_list_is_valid_deep(list) == true

Postconditions (validity):
  - The returned pointer r is equal to list->tail.prev
  - r may be the list head sentinel when the list is empty
  - No NULL dereference occurs inside the function

Postconditions (frame):
  - The contents of *list (head, tail, and their next/prev links) are unchanged
  - No memory other than the return value is modified
===*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void aws_linked_list_rbegin_harness(void) {
    /* Allocate and initialize a linked list */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    assert(list != NULL);
    aws_linked_list_init(list);

    /* Build a nondeterministic but valid list */
    size_t max_nodes = 5;
    size_t num_nodes = nondet_uint() % (max_nodes + 1);
    struct aws_linked_list_node *nodes[6]; /* one extra for safety */

    for (size_t i = 0; i < num_nodes; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        assert(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);
        aws_linked_list_push_back(list, nodes[i]);
    }

    /* Assume the list is valid after construction */
    __CPROVER_assume(aws_linked_list_is_valid(list));
    __CPROVER_assume(aws_linked_list_is_valid_deep(list));

    /* Capture the pre‑state of the list structure */
    struct aws_linked_list old_list = *list;

    /* Call the function under test */
    struct aws_linked_list_node *r = aws_linked_list_rbegin(list);

    /* Postcondition: return value equals tail.prev */
    assert(r == list->tail.prev);

    /* Frame condition: the list structure is unchanged */
    assert(memcmp(&old_list, list, sizeof(struct aws_linked_list)) == 0);

    /* Clean up */
    for (size_t i = 0; i < num_nodes; ++i) {
        free(nodes[i]);
    }
    free(list);
    return 0;
}
