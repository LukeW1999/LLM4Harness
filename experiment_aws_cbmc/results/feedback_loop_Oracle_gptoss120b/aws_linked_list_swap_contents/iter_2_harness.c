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
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list a;
    struct aws_linked_list b;
    aws_linked_list_init(&a);
    aws_linked_list_init(&b);

    struct aws_linked_list_node *orig_a_nodes[MAX_NODES];
    struct aws_linked_list_node *orig_b_nodes[MAX_NODES];
    size_t orig_a_cnt = 0;
    size_t orig_b_cnt = 0;

    for (size_t i = 0; i < MAX_NODES; ++i) {
        bool in_a = __CPROVER_nondet_bool();
        bool in_b = __CPROVER_nondet_bool();
        __CPROVER_assume(!(in_a && in_b)); /* a node cannot belong to both lists */

        if (in_a) {
            struct aws_linked_list_node *node =
                (struct aws_linked_list_node *)aws_mem_acquire(allocator, sizeof(*node));
            __CPROVER_assume(node != NULL);
            node->next = NULL;
            node->prev = NULL;
            orig_a_nodes[orig_a_cnt++] = node;
            aws_linked_list_push_back(&a, node);
        }

        if (in_b) {
            struct aws_linked_list_node *node =
                (struct aws_linked_list_node *)aws_mem_acquire(allocator, sizeof(*node));
            __CPROVER_assume(node != NULL);
            node->next = NULL;
            node->prev = NULL;
            orig_b_nodes[orig_b_cnt++] = node;
            aws_linked_list_push_back(&b, node);
        }
    }

    aws_linked_list_swap_contents(&a, &b);

    /* post‑conditions */
    assert(aws_linked_list_is_valid(&a));
    assert(aws_linked_list_is_valid(&b));
    assert(aws_linked_list_is_valid_deep(&a));
    assert(aws_linked_list_is_valid_deep(&b));

    {
        size_t cnt_a = 0, cnt_b = 0;
        struct aws_linked_list_node *cur;

        for (cur = aws_linked_list_begin(&a); cur != aws_linked_list_end(&a);
             cur = aws_linked_list_next(cur)) {
            ++cnt_a;
        }
        for (cur = aws_linked_list_begin(&b); cur != aws_linked_list_end(&b);
             cur = aws_linked_list_next(cur)) {
            ++cnt_b;
        }
        assert(cnt_a + cnt_b == orig_a_cnt + orig_b_cnt);
    }

    {
        size_t found = 0;
        struct aws_linked_list_node *cur;
        for (cur = aws_linked_list_begin(&a); cur != aws_linked_list_end(&a);
             cur = aws_linked_list_next(cur)) {
            bool present = false;
            for (size_t j = 0; j < orig_b_cnt; ++j) {
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

    {
        size_t found = 0;
        struct aws_linked_list_node *cur;
        for (cur = aws_linked_list_begin(&b); cur != aws_linked_list_end(&b);
             cur = aws_linked_list_next(cur)) {
            bool present = false;
            for (size_t j = 0; j < orig_a_cnt; ++j) {
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
}
