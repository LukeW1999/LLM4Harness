/*=== Contract for aws_linked_list_init ===
Preconditions:
  - __CPROVER_assume(list != NULL);   // pointer to a struct aws_linked_list must be non‑NULL

Postconditions (validity):
  - assert(list->head.next == &list->tail);
  - assert(list->head.prev == NULL);
  - assert(list->tail.prev == &list->head);
  - assert(list->tail.next == NULL);
  - assert(aws_linked_list_is_valid(list));
  - assert(aws_linked_list_empty(list));

Postconditions (frame):
  - No memory outside of *list is modified.
*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_init_harness(void) {
    /* Allocate a list object with nondet contents */
    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);

    /* The object may contain arbitrary data; CBMC treats malloc memory as nondet */

    /* Call the function under verification */
    aws_linked_list_init(list);

    /* Verify postconditions */
    assert(list->head.next == &list->tail);
    assert(list->head.prev == NULL);
    assert(list->tail.prev == &list->head);
    assert(list->tail.next == NULL);
    assert(aws_linked_list_is_valid(list));
    assert(aws_linked_list_empty(list));

    return 0;
}
