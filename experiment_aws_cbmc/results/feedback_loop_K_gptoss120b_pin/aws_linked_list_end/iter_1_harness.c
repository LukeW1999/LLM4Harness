/*  
Preconditions:  
- list != NULL  
- aws_linked_list_is_valid(list) holds (the list is a well‑formed doubly linked list)  

Postconditions (validity):  
- The returned pointer is non‑NULL  
- The returned pointer is exactly &list->tail  

Postconditions (frame):  
- The memory of *list (including its head and tail nodes) is unchanged by the call  
*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void aws_linked_list_end_harness(void) {
    /* Allocate a list structure */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);

    /* Nondeterministically initialize the list */
    nondet_init_struct_aws_linked_list(list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Preserve a copy of the list for frame checking */
    struct aws_linked_list list_before;
    memcpy(&list_before, list, sizeof(struct aws_linked_list));

    /* Call the function under test */
    const struct aws_linked_list_node *end_node = aws_linked_list_end(list);

    /* Postcondition: returned pointer is non‑NULL */
    assert(end_node != NULL);

    /* Postcondition: returned pointer equals &list->tail */
    assert(end_node == &list->tail);

    /* Frame condition: the list structure has not been modified */
    assert(memcmp(&list_before, list, sizeof(struct aws_linked_list)) == 0);

    /* Clean up */
    free(list);
    return 0;
}
