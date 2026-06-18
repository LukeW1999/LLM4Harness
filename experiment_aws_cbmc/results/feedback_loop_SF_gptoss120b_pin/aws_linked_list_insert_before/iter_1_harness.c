#include "aws/common/common.h"
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* Allocate and initialize a linked list */
    struct aws_linked_list *list = (struct aws_linked_list *)malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(list != NULL);
    aws_linked_list_init(list);
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Allocate a node that will serve as 'before' and insert it into the list */
    struct aws_linked_list_node *before = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(before != NULL);
    aws_linked_list_push_back(list, before);
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* Allocate a node that will be added before 'before' */
    struct aws_linked_list_node *to_add = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);
    aws_linked_list_node_reset(to_add);
    __CPROVER_assume(!aws_linked_list_node_is_in_list(to_add));

    /* Snapshot pre‑state: length of the list */
    size_t pre_len = 0;
    for (struct aws_linked_list_node *cur = aws_linked_list_begin(list);
         cur != aws_linked_list_end(list);
         cur = aws_linked_list_next(cur)) {
        pre_len++;
    }

    /* Call the function under verification */
    aws_linked_list_insert_before(before, to_add);

    /* ASSERT_POSTCONDITIONS_HERE */
    /* List must remain valid */
    assert(aws_linked_list_is_valid(list));

    /* The newly added node must now be part of the list */
    assert(aws_linked_list_node_is_in_list(to_add));

    /* The 'before' node's previous pointer must now point to the newly added node */
    assert(before->prev == to_add);

    /* The newly added node must point forward to the 'before' node */
    assert(to_add->next == before);

    /* Length of the list must have increased by exactly one */
    {
        size_t post_len = 0;
        for (struct aws_linked_list_node *cur = aws_linked_list_begin(list);
             cur != aws_linked_list_end(list);
             cur = aws_linked_list_next(cur)) {
            post_len++;
        }
        assert(post_len == pre_len + 1);
    }
}
