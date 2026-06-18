/*  
Preconditions:  
- __CPROVER_assume(a != NULL);  
- __CPROVER_assume(b != NULL);  
- __CPROVER_assume(a != b);  // restrict aliasing  
- __CPROVER_assume(aws_linked_list_is_valid(a));  
- __CPROVER_assume(aws_linked_list_is_valid(b));  
- __CPROVER_assume(aws_linked_list_is_valid_deep(a));  
- __CPROVER_assume(aws_linked_list_is_valid_deep(b));  

Postconditions (validity):  
- assert(aws_linked_list_is_valid(a));  
- assert(aws_linked_list_is_valid(b));  
- assert(aws_linked_list_is_valid_deep(a));  
- assert(aws_linked_list_is_valid_deep(b));  

Postconditions (length):  
- Let len_a_before = length of list a before the call.  
- Let len_b_before = length of list b before the call.  
- After the call, length of a == len_b_before and length of b == len_a_before.  

Postconditions (frame):  
- No memory outside the allocated lists and their nodes is modified.  
*/

#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

/* Helper to compute list length */
static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    struct aws_linked_list_node *cur = aws_linked_list_begin(list);
    const struct aws_linked_list_node *end = aws_linked_list_end(list);
    while (cur != end) {
        len++;
        cur = aws_linked_list_next(cur);
    }
    return len;
}

/* Helper to allocate a list and optionally fill it with nodes */
static void make_list(struct aws_linked_list *list, size_t max_nodes) {
    aws_linked_list_init(list);
    size_t n = (size_t) nondet_uint();
    __CPROVER_assume(n <= max_nodes);
    for (size_t i = 0; i < n; ++i) {
        struct aws_linked_list_node *node = (struct aws_linked_list_node *)malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        aws_linked_list_push_back(list, node);
    }
}

/* Helper to collect all node pointers from a list into an array */
static void collect_nodes(const struct aws_linked_list *list, struct aws_linked_list_node **arr, size_t *cnt) {
    size_t idx = 0;
    struct aws_linked_list_node *cur = aws_linked_list_begin(list);
    const struct aws_linked_list_node *end = aws_linked_list_end(list);
    while (cur != end) {
        arr[idx++] = cur;
        cur = aws_linked_list_next(cur);
    }
    *cnt = idx;
}

/* Harness */
void aws_linked_list_swap_contents_harness(void) {
    /* Allocate two distinct lists */
    struct aws_linked_list *a = (struct aws_linked_list *)malloc(sizeof(struct aws_linked_list));
    struct aws_linked_list *b = (struct aws_linked_list *)malloc(sizeof(struct aws_linked_list));
    __CPROVER_assume(a != NULL);
    __CPROVER_assume(b != NULL);
    __CPROVER_assume(a != b);

    /* Build lists with nondet number of nodes (bounded) */
    const size_t MAX_NODES = 5;
    make_list(a, MAX_NODES);
    make_list(b, MAX_NODES);

    /* Record pre‑state lengths */
    size_t len_a_before = list_length(a);
    size_t len_b_before = list_length(b);

    /* Record pre‑state node sets */
    struct aws_linked_list_node *a_nodes[ MAX_NODES ];
    struct aws_linked_list_node *b_nodes[ MAX_NODES ];
    size_t a_cnt = 0, b_cnt = 0;
    collect_nodes(a, a_nodes, &a_cnt);
    collect_nodes(b, b_nodes, &b_cnt);

    /* Assume the lists are valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(a));
    __CPROVER_assume(aws_linked_list_is_valid(b));
    __CPROVER_assume(aws_linked_list_is_valid_deep(a));
    __CPROVER_assume(aws_linked_list_is_valid_deep(b));

    /* Call the function under test */
    aws_linked_list_swap_contents(a, b);

    /* Post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(a));
    assert(aws_linked_list_is_valid(b));
    assert(aws_linked_list_is_valid_deep(a));
    assert(aws_linked_list_is_valid_deep(b));

    /* Post‑condition: lengths are swapped */
    assert(list_length(a) == len_b_before);
    assert(list_length(b) == len_a_before);

    /* Post‑condition: the set of nodes is unchanged (only ownership swapped) */
    {
        struct aws_linked_list_node *new_a_nodes[ MAX_NODES ];
        struct aws_linked_list_node *new_b_nodes[ MAX_NODES ];
        size_t new_a_cnt = 0, new_b_cnt = 0;
        collect_nodes(a, new_a_nodes, &new_a_cnt);
        collect_nodes(b, new_b_nodes, &new_b_cnt);
        assert(new_a_cnt == b_cnt);
        assert(new_b_cnt == a_cnt);
        for (size_t i = 0; i < new_a_cnt; ++i) {
            bool found = false;
            for (size_t j = 0; j < b_cnt; ++j) {
                if (new_a_nodes[i] == b_nodes[j]) { found = true; break; }
            }
            assert(found);
        }
        for (size_t i = 0; i < new_b_cnt; ++i) {
            bool found = false;
            for (size_t j = 0; j < a_cnt; ++j) {
                if (new_b_nodes[i] == a_nodes[j]) { found = true; break; }
            }
            assert(found);
        }
    }

    return 0;
}
