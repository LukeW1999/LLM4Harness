#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>

#include <proof_helpers/make_common_data_structures.h>

#define MAX_NODES 5

void aws_linked_list_swap_contents_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* linked lists */
    struct aws_linked_list a;
    struct aws_linked_list b;
    aws_linked_list_init(&a);
    aws_linked_list_init(&b);

    /* original node sets */
    struct aws_linked_list_node *orig_a_nodes[MAX_NODES];
    struct aws_linked_list_node *orig_b_nodes[MAX_NODES];
    size_t orig_a_cnt = 0;
    size_t orig_b_cnt = 0;

    /* nondet decide how many nodes go into each list */
    size_t i;
    for (i = 0; i < MAX_NODES; ++i) {
        bool in_a = __CPROVER_nondet_bool();
        bool in_b = __CPROVER_nondet_bool();

        /* allocate a node if it will be used */
        if (in_a || in_b) {
            struct aws_linked_list_node *node = (struct aws_linked_list_node *)aws_mem_acquire(allocator, sizeof(struct aws_linked_list_node));
            __CPROVER_assume(node != NULL);
            node->next = NULL;
            node->prev = NULL;

            if (in_a) {
                orig_a_nodes[orig_a_cnt++] = node;
                aws_linked_list_push_back(&a, node);
            }
            if (in_b) {
                orig_b_nodes[orig_b_cnt++] = node;
                aws_linked_list_push_back(&b, node);
            }
        }
    }

    /* remember original heads/tails for frame condition checks */
    struct aws_linked_list_node *a_head_next_orig = a.head.next;
    struct aws_linked_list_node *a_tail_prev_orig = a.tail.prev;
    struct aws_linked_list_node *b_head_next_orig = b.head.next;
    struct aws_linked_list_node *b_tail_prev_orig = b.tail.prev;

    /* call function under test */
    aws_linked_list_swap_contents(&a, &b);

    /* post‑conditions */

    /* both lists must be valid */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));

    /* total number of nodes must be unchanged */
    {
        size_t cnt_a = 0, cnt_b = 0;
        struct aws_linked_list_node *cur;

        for (cur = aws_linked_list_begin(&a); cur != aws_linked_list_end(&a); cur = aws_linked_list_next(cur)) {
            ++cnt_a;
        }
        for (cur = aws_linked_list_begin(&b); cur != aws_linked_list_end(&b); cur = aws_linked_list_next(cur)) {
            ++cnt_b;
        }
        assert(cnt_a + cnt_b == orig_a_cnt + orig_b_cnt);
    }

    /* the set of nodes in a after the swap must equal the original set of nodes in b */
    {
        size_t found = 0;
        struct aws_linked_list_node *cur;
        for (cur = aws_linked_list_begin(&a); cur != aws_linked_list_end(&a); cur = aws_linked_list_next(cur)) {
            size_t j;
            bool present = false;
            for (j = 0; j < orig_b_cnt; ++j) {
                if (cur == orig_b_nodes[j]) {
                    present = true;
                    break;
                }
            }
            assert(present);
            ++found;
        }
        assert(found == orig_b_cnt);
    }

    /* the set of nodes in b after the swap must equal the original set of nodes in a */
    {
        size_t found = 0;
        struct aws_linked_list_node *cur;
        for (cur = aws_linked_list_begin(&b); cur != aws_linked_list_end(&b); cur = aws_linked_list_next(cur)) {
            size_t j;
            bool present = false;
            for (j = 0; j < orig_a_cnt; ++j) {
                if (cur == orig_a_nodes[j]) {
                    present = true;
                    break;
                }
            }
            assert(present);
            ++found;
        }
        assert(found == orig_a_cnt);
    }

    /* frame condition: list head/tail pointers that were not part of the swap must remain unchanged */
    assert(a_head_next_orig == (orig_a_cnt == 0 ? &a.tail : a_head_next_orig));
    assert(a_tail_prev_orig == (orig_a_cnt == 0 ? &a.head : a_tail_prev_orig));
    assert(b_head_next_orig == (orig_b_cnt == 0 ? &b.tail : b_head_next_orig));
    assert(b_tail_prev_orig == (orig_b_cnt == 0 ? &b.head : b_tail_prev_orig));

    return 0;
}
