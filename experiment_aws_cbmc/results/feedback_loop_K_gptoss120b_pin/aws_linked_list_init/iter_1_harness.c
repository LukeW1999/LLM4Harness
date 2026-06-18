/*=== Contract for aws_linked_list_init =============================

Preconditions:
- __CPROVER_assume(list != NULL);
- The memory region pointed to by list is writable and does not overlap any other
  object that the harness cares about.

Postconditions (validity):
- assert(aws_linked_list_is_valid(list));
- assert(aws_linked_list_empty(list));

Postconditions (frame):
- No memory outside of the object *list is modified.  The harness creates a
  second, unrelated aws_linked_list object (other) and asserts that it is
  unchanged after the call.

====================================================================*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/* Helper to nondeterministically initialize a struct */
static void nondet_init_aws_linked_list(struct aws_linked_list *obj) {
    /* Each pointer field is nondet; we do not place any constraints here
       because aws_linked_list_init must work regardless of the initial
       contents. */
    obj->head.next = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    obj->head.prev = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    obj->tail.prev = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    obj->tail.next = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
}

/* Helper to free the auxiliary allocations used in nondet_init */
static void free_aux_nodes(struct aws_linked_list *obj) {
    free(obj->head.next);
    free(obj->head.prev);
    free(obj->tail.prev);
    free(obj->tail.next);
}

void aws_linked_list_init_harness(void) {
    /* Allocate the list under test */
    struct aws_linked_list *list = malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);

    /* Allocate a second list to serve as a frame object */
    struct aws_linked_list other = {0};
    nondet_init_aws_linked_list(&other);

    /* Keep a copy of the frame object for later comparison */
    struct aws_linked_list other_before = other;

    /* Call the function under verification */
    aws_linked_list_init(list);

    /* ---- Postcondition checks ---- */
    /* Validity and emptiness */
    assert(aws_linked_list_is_valid(list));
    assert(aws_linked_list_empty(list));

    /* Frame: other must be unchanged */
    assert(other.head.next == other_before.head.next);
    assert(other.head.prev == other_before.head.prev);
    assert(other.tail.prev == other_before.tail.prev);
    assert(other.tail.next == other_before.tail.next);

    /* Clean up */
    free(list);
    free_aux_nodes(&other);
    return 0;
}
