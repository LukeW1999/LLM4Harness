/*  
Preconditions:  
- __CPROVER_assume(list != NULL);  
- __CPROVER_assume(aws_linked_list_is_valid(list));  

Postconditions (validity):  
- The returned pointer is exactly list->head.next.  

Postconditions (frame):  
- The memory of *list (including its head and tail nodes) is not modified by the call.  
- The list remains a valid linked list after the call (aws_linked_list_is_valid(list) holds).  
*/

#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_linked_list_begin_harness(void) {
    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);

    /* nondet initialize the list structure */
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Preserve a copy of the list for frame checking */
    struct aws_linked_list list_before;
    memcpy(&list_before, list, sizeof(*list));

    struct aws_linked_list_node *result = aws_linked_list_begin(list);

    /* Postcondition: return value equals list->head.next */
    assert(result == list->head.next);

    /* Frame condition: list memory unchanged */
    assert(memcmp(list, &list_before, sizeof(*list)) == 0);

    /* List remains valid after the call */
    assert(aws_linked_list_is_valid(list));

    return 0;
}
