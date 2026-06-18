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

    /* Preconditions: both lists must be valid */
    __CPROVER_assume(aws_linked_list_is_valid(&a));
    __CPROVER_assume(aws_linked_list_is_valid(&b));

    /* Call the function under verification */
    aws_linked_list_swap_contents(&a, &b);

    /* Postcondition assertions */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
}
