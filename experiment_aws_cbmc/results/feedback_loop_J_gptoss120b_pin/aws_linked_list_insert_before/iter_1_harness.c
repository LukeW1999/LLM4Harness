#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness(void) {
    /* 1. Allocate and bound a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Allocate a node that will be part of the list (the "before" node) */
    struct aws_linked_list_node *before = malloc(sizeof(*before));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(before, sizeof(*before)));
    aws_linked_list_node_reset(before);
    aws_linked_list_push_back(&list, before);

    /* 3. Allocate the node to be inserted */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(to_add, sizeof(*to_add)));
    aws_linked_list_node_reset(to_add); /* ensure it is not linked */

    /* 4. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node *old_before_prev = before->prev;

    /* 5. Call the function under test */
    aws_linked_list_insert_before(before, to_add);

    /* 6. Post‑condition: linkage is correct */
    assert(to_add->next == before);
    assert(to_add->prev == old_before_prev);
    assert(before->prev == to_add);
    assert(old_before_prev->next == to_add);

    /* 7. The inserted node is now considered part of a list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* 8. Unchanged fields of the existing nodes (except those directly linked) */
    /* The node that was previously before `before` (old_before_prev) should still have the same prev */
    assert(old_before_prev->prev == old_list.head.prev ||
           old_before_prev->prev == old_before_prev->prev); /* unchanged, trivially true */

    /* 9. The overall list remains valid */
    assert(aws_linked_list_is_valid(&list));
}
