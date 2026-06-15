/*  
 * Contract for aws_linked_list_rend  
 * Preconditions:  
 *   - list != NULL  
 *   - aws_linked_list_is_valid(list) == true (the list satisfies all internal invariants)  
 * Postconditions (validity):  
 *   - return value != NULL  
 *   - return value == &list->head (the sentinel node before the first element)  
 * Postconditions (frame):  
 *   - The function does not modify any memory of the list (head, tail, or any nodes)  
 */

#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_linked_list_rend_harness(void) {
    /* Allocate and initialize a linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Optionally populate the list with a nondeterministic number of nodes */
    const uint32_t max_nodes = 5;
    uint32_t node_count = __CPROVER_nondet_uint();
    __CPROVER_assume(node_count <= max_nodes);

    struct aws_linked_list_node *nodes = NULL;
    if (node_count > 0) {
        nodes = (struct aws_linked_list_node *)malloc(node_count * sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes != NULL);
        for (uint32_t i = 0; i < node_count; ++i) {
            aws_linked_list_node_reset(&nodes[i]);
            aws_linked_list_push_back(&list, &nodes[i]);
        }
    }

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Snapshot the list state to check the frame condition */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_nodes = NULL;
    if (node_count > 0) {
        old_nodes = (struct aws_linked_list_node *)malloc(node_count * sizeof(struct aws_linked_list_node));
        __CPROVER_assume(old_nodes != NULL);
        memcpy(old_nodes, nodes, node_count * sizeof(struct aws_linked_list_node));
    }

    /* Call the function under test */
    const struct aws_linked_list_node *r = aws_linked_list_rend(&list);

    /* Postcondition: return value is non‑NULL */
    assert(r != NULL);

    /* Postcondition: return value points to the list head sentinel */
    assert(r == &list.head);

    /* Frame condition: the list structure itself is unchanged */
    assert(memcmp(&list, &old_list, sizeof(struct aws_linked_list)) == 0);

    /* Frame condition: the nodes themselves are unchanged */
    if (node_count > 0) {
        assert(memcmp(nodes, old_nodes, node_count * sizeof(struct aws_linked_list_node)) == 0);
    }

    /* Clean up */
    free(nodes);
    free(old_nodes);

    return 0;
}
