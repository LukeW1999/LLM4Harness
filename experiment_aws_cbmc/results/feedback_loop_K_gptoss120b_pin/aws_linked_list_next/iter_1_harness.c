/*  
Preconditions:  
- __CPROVER_assume(node != NULL);  
- __CPROVER_assume(aws_linked_list_is_valid(list)); // the list containing the node is a valid doubly‑linked list  
- __CPROVER_assume(node is either a list element, the head sentinel, or the tail sentinel of that list);  

Postconditions (validity):  
- The returned pointer is exactly node->next (may be NULL or a sentinel).  

Postconditions (frame):  
- No memory locations are modified; the list remains valid after the call (aws_linked_list_is_valid and aws_linked_list_is_valid_deep still hold).  
*/

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_next_harness(void) {
    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Create a bounded number of nodes and push them onto the list */
    const size_t MAX_NODES = 5;
    struct aws_linked_list_node *nodes[MAX_NODES];

    for (size_t i = 0; i < MAX_NODES; ++i) {
        nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(nodes[i] != NULL);
        aws_linked_list_node_reset(nodes[i]);
        aws_linked_list_push_back(&list, nodes[i]);
    }

    /* Choose a node nondeterministically: could be any list element or a sentinel */
    struct aws_linked_list_node *chosen;
    __CPROVER_assume(
        chosen == aws_linked_list_begin(&list) ||
        chosen == aws_linked_list_end(&list) ||
        chosen == aws_linked_list_rbegin(&list) ||
        chosen == aws_linked_list_rend(&list) ||
        chosen == nodes[0] ||
        chosen == nodes[1] ||
        chosen == nodes[2] ||
        chosen == nodes[3] ||
        chosen == nodes[4]);

    /* Preconditions */
    __CPROVER_assume(chosen != NULL);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Capture expected result */
    struct aws_linked_list_node *expected = chosen->next;

    /* Call the function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(chosen);

    /* Postconditions */
    assert(result == expected);

    /* Frame conditions: the list must remain valid and unchanged */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_is_valid_deep(&list));

    return 0;
}
