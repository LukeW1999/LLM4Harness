#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_insert_after_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize an empty list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Allocate the node that will be the "after" node */
    struct aws_linked_list_node *after = malloc(sizeof(*after));
    __CPROVER_assume(after != NULL);
    aws_linked_list_node_reset(after);

    /* Allocate the node that will be inserted */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);

    /* Build a list that contains at least the "after" node */
    aws_linked_list_push_back(&list, after);
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(aws_linked_list_node_is_in_list(after));

    /* Ensure the node to be inserted is not already in a list */
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* Additional precondition: after->next must be a valid pointer */
    __CPROVER_assume(after->next != NULL);

    /* Snapshot of relevant state before the call */
    struct aws_linked_list_node *orig_after_next = after->next;
    size_t orig_len = 0;
    for (struct aws_linked_list_node *it = aws_linked_list_begin(&list);
         it != aws_linked_list_end(&list);
         it = aws_linked_list_next(it)) {
        orig_len++;
    }

    /* Call the function under verification */
    aws_linked_list_insert_after(after, to_add);

    
}
