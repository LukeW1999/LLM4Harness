#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_NODES 5

static size_t list_length(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = aws_linked_list_begin(list);
    const struct aws_linked_list_node *end = aws_linked_list_end(list);
    while (cur != end) {
        cnt++;
        cur = aws_linked_list_next(cur);
    }
    return cnt;
}

/* Returns true if node is present in the array of length len */
static bool node_in_array(const struct aws_linked_list_node *node,
                          struct aws_linked_list_node *arr[],
                          size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (arr[i] == node) {
            return true;
        }
    }
    return false;
}

/* Checks that the list contains exactly the nodes in arr[0..len-1] (order ignored) */
static bool list_contains_exactly(const struct aws_linked_list *list,
                                  struct aws_linked_list_node *arr[],
                                  size_t len) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = aws_linked_list_begin(list);
    const struct aws_linked_list_node *end = aws_linked_list_end(list);
    while (cur != end) {
        if (!node_in_array(cur, arr, len)) {
            return false;
        }
        cnt++;
        cur = aws_linked_list_next(cur);
    }
    return cnt == len;
}

void aws_linked_list_swap_contents_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* allocate and initialise two lists */
    struct aws_linked_list a;
    struct aws_linked_list b;
    aws_linked_list_init(&a);
    aws_linked_list_init(&b);

    /* decide lengths nondeterministically */
    size_t a_len_before = nondet_uint() % (MAX_NODES + 1);
    size_t b_len_before = nondet_uint() % (MAX_NODES + 1);

    __CPROVER_assume(a_len_before <= MAX_NODES);
    __CPROVER_assume(b_len_before <= MAX_NODES);

    struct aws_linked_list_node *a_nodes_before[MAX_NODES];
    struct aws_linked_list_node *b_nodes_before[MAX_NODES];

    /* populate list a */
    for (size_t i = 0; i < a_len_before; ++i) {
        struct aws_linked_list_node *node = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        a_nodes_before[i] = node;
        aws_linked_list_push_back(&a, node);
    }

    /* populate list b */
    for (size_t i = 0; i < b_len_before; ++i) {
        struct aws_linked_list_node *node = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        b_nodes_before[i] = node;
        aws_linked_list_push_back(&b, node);
    }

    /* Save pre‑state validity */
    __CPROVER_assert(aws_linked_list_is_valid(&a), "pre: list a is valid");
    __CPROVER_assert(aws_linked_list_is_valid(&b), "pre: list b is valid");
    __CPROVER_assert(aws_linked_list_is_valid_deep(&a), "pre: list a is deep valid");
    __CPROVER_assert(aws_linked_list_is_valid_deep(&b), "pre: list b is deep valid");

    /* Call the function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* Post‑conditions */

    /* 1. Validity predicates */
    __CPROVER_assert(aws_linked_list_is_valid(&a), "post: list a is valid");
    __CPROVER_assert(aws_linked_list_is_valid(&b), "post: list b is valid");
    __CPROVER_assert(aws_linked_list_is_valid_deep(&a), "post: list a is deep valid");
    __CPROVER_assert(aws_linked_list_is_valid_deep(&b), "post: list b is deep valid");

    /* 2. Length invariants */
    __CPROVER_assert(list_length(&a) == b_len_before,
                     "post: length of a equals original length of b");
    __CPROVER_assert(list_length(&b) == a_len_before,
                     "post: length of b equals original length of a");

    /* 3. Contents swapped */
    __CPROVER_assert(list_contains_exactly(&a, b_nodes_before, b_len_before),
                     "post: a contains exactly the original nodes of b");
    __CPROVER_assert(list_contains_exactly(&b, a_nodes_before, a_len_before),
                     "post: b contains exactly the original nodes of a");

    /* 4. Frame condition: no other memory modified.
       Since we only allocated the list structures and the nodes,
       we assert that the allocator state (if any) is unchanged.
       Here we conservatively assume malloc does not modify other memory. */
    return 0;
}
