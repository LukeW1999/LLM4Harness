/*=== Contract ===
Preconditions:
  - a != NULL
  - b != NULL
  - a != b                         // due to AWS_RESTRICT
  - aws_linked_list_is_valid(a)
  - aws_linked_list_is_valid(b)
  - All nodes reachable from a and b are distinct (no node appears in both lists)
Postconditions (validity):
  - aws_linked_list_is_valid(a) after the call
  - aws_linked_list_is_valid(b) after the call
  - aws_linked_list_is_valid_deep(a) after the call
  - aws_linked_list_is_valid_deep(b) after the call
Postconditions (length):
  - length of a after the call equals length of b before the call
  - length of b after the call equals length of a before the call
Postconditions (frame):
  - No memory outside the allocated lists (including the allocator) is modified
===*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

/* Helper to compute the length of a list */
static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    for (struct aws_linked_list_node *node = aws_linked_list_begin(list);
         node != aws_linked_list_end(list);
         node = aws_linked_list_next(node)) {
        len++;
    }
    return len;
}

/* Helper to allocate a node and reset it */
static struct aws_linked_list_node *alloc_node(void) {
    struct aws_linked_list_node *node = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);
    aws_linked_list_node_reset(node);
    return node;
}

/* Helper to create a list with a nondet number of nodes (0..MAX_NODES) */
static void make_list(struct aws_linked_list *list, size_t max_nodes) {
    aws_linked_list_init(list);
    size_t n = (size_t)nondet_uint();
    __CPROVER_assume(n <= max_nodes);
    for (size_t i = 0; i < n; ++i) {
        struct aws_linked_list_node *node = alloc_node();
        aws_linked_list_push_back(list, node);
    }
}

/* Helper to ensure two lists do not share any node */
static void assume_no_shared_nodes(const struct aws_linked_list *a, const struct aws_linked_list *b) {
    for (struct aws_linked_list_node *na = aws_linked_list_begin(a);
         na != aws_linked_list_end(a);
         na = aws_linked_list_next(na)) {
        for (struct aws_linked_list_node *nb = aws_linked_list_begin(b);
             nb != aws_linked_list_end(b);
             nb = aws_linked_list_next(nb)) {
            __CPROVER_assume(na != nb);
        }
    }
}

/* Main harness */
void aws_linked_list_swap_contents_harness(void) {
    /* Allocate two distinct lists */
    struct aws_linked_list *a = (struct aws_linked_list *)malloc(sizeof(struct aws_linked_list));
    struct aws_linked_list *b = (struct aws_linked_list *)malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(a != NULL);
    __CPROVER_assume(b != NULL);
    __CPROVER_assume(a != b);                     // AWS_RESTRICT guarantee

    /* Build lists with up to 5 nodes each */
    const size_t MAX_NODES = 5;
    make_list(a, MAX_NODES);
    make_list(b, MAX_NODES);

    /* Ensure the lists are initially valid and disjoint */
    __CPROVER_assume(aws_linked_list_is_valid(a));
    __CPROVER_assume(aws_linked_list_is_valid(b));
    assume_no_shared_nodes(a, b);

    /* Record lengths before the swap */
    size_t len_a_before = list_length(a);
    size_t len_b_before = list_length(b);

    /* Call the function under test */
    aws_linked_list_swap_contents(a, b);

    /* Postcondition checks */
    assert(aws_linked_list_is_valid(a));
    assert(aws_linked_list_is_valid(b));
    assert(aws_linked_list_is_valid_deep(a));
    assert(aws_linked_list_is_valid_deep(b));

    /* Lengths must be swapped */
    assert(list_length(a) == len_b_before);
    assert(list_length(b) == len_a_before);

    return 0;
}
