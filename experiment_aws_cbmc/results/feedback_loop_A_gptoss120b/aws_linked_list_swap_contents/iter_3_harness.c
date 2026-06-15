#include <assert.h>
#include <stdlib.h>
#include "aws/common/linked_list.h"
#include "proof_helpers/make_common_data_structures.h"

struct test_node {
    struct aws_linked_list_node node;
    int data;
};

void aws_linked_list_swap_contents_harness(void) {
    struct aws_linked_list a;
    struct aws_linked_list b;

    /* Allocate and initialise the lists */
    ensure_linked_list_is_allocated(&a, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&b, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&a);
    aws_linked_list_init(&b);

    /* Possibly add a single element to each list */
    if (__CPROVER_nondet_bool()) {
        struct test_node *n = malloc(sizeof(*n));
        __CPROVER_assume(n != NULL);
        aws_linked_list_push_back(&a, &n->node);
    }
    if (__CPROVER_nondet_bool()) {
        struct test_node *n = malloc(sizeof(*n));
        __CPROVER_assume(n != NULL);
        aws_linked_list_push_back(&b, &n->node);
    }

    /* Record pre‑swap state */
    size_t len_a_before = aws_linked_list_length(&a);
    size_t len_b_before = aws_linked_list_length(&b);
    struct aws_linked_list_node *first_a_before = aws_linked_list_begin(&a);
    struct aws_linked_list_node *first_b_before = aws_linked_list_begin(&b);

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Record post‑swap state */
    size_t len_a_after = aws_linked_list_length(&a);
    size_t len_b_after = aws_linked_list_length(&b);
    struct aws_linked_list_node *first_a_after = aws_linked_list_begin(&a);
    struct aws_linked_list_node *first_b_after = aws_linked_list_begin(&b);

    /* Verify that lengths have been swapped */
    assert(len_a_after == len_b_before);
    assert(len_b_after == len_a_before);

    /* Verify that the first nodes have been swapped when both lists were non‑empty */
    if (len_a_before > 0 && len_b_before > 0) {
        assert(first_a_after == first_b_before);
        assert(first_b_after == first_a_before);
    }
}
