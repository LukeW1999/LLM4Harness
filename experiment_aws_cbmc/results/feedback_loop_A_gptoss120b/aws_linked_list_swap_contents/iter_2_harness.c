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
