#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_swap_contents_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_linked_list a;
    struct aws_linked_list b;

    /* Initialize the lists */
    aws_linked_list_init(&a);
    aws_linked_list_init(&b);

    /* Nondeterministically populate the lists with a bounded number of nodes */
    size_t a_node_count = nondet_uint();
    size_t b_node_count = nondet_uint();
    __CPROVER_assume(a_node_count <= 10);
    __CPROVER_assume(b_node_count <= 10);

    make_linked_list(alloc, &a, a_node_count);
    make_linked_list(alloc, &b, b_node_count);

    /* Preconditions: both lists must be valid and distinct */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Snapshot of input state */
    struct aws_linked_list_node *a_head_next_snapshot = a.head.next;
    struct aws_linked_list_node *a_tail_prev_snapshot = a.tail.prev;
    struct aws_linked_list_node *b_head_next_snapshot = b.head.next;
    struct aws_linked_list_node *b_tail_prev_snapshot = b.tail.prev;
    size_t a_len_snapshot = a_node_count;
    size_t b_len_snapshot = b_node_count;

    /* Call the function under verification */
    aws_linked_list_swap_contents(&a, &b);

    
}
