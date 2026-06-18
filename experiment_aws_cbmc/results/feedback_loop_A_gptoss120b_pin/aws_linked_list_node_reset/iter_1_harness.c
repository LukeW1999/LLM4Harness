#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

/* Helper to count nodes in a list */
static size_t count_nodes(const struct aws_linked_list *list) {
    size_t cnt = 0;
    for (struct aws_linked_list_node *cur = list->head.next; cur != &list->tail; cur = cur->next) {
        ++cnt;
    }
    return cnt;
}

void aws_linked_list_move_all_back_harness() {
    /* allocate and initialise two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    aws_linked_list_init(&dst);
    aws_linked_list_init(&src);

    /* nondet lengths bounded by the maximum allocation */
    size_t len_dst = nondet_size_t();
    size_t len_src = nondet_size_t();
    __CPROVER_assume(len_dst <= MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(len_src <= MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* allocate nodes for dst and push_back */
    struct aws_linked_list_node *dst_nodes[MAX_LINKED_LIST_ITEM_ALLOCATION];
    for (size_t i = 0; i < len_dst; ++i) {
        dst_nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(dst_nodes[i] != NULL);
        aws_linked_list_push_back(&dst, dst_nodes[i]);
    }

    /* allocate nodes for src and push_back */
    struct aws_linked_list_node *src_nodes[MAX_LINKED_LIST_ITEM_ALLOCATION];
    for (size_t i = 0; i < len_src; ++i) {
        src_nodes[i] = malloc(sizeof(struct aws_linked_list_node));
        __CPROVER_assume(src_nodes[i] != NULL);
        aws_linked_list_push_back(&src, src_nodes[i]);
    }

    /* assume both lists are valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    /* save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    /* call function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* post‑condition: both lists remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* post‑condition: src is now empty */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);

    /* post‑condition: dst length equals old lengths summed */
    assert(count_nodes(&dst) == len_dst + len_src);

    /* post‑condition: original dst nodes remain in original order */
    {
        struct aws_linked_list_node *cur = dst.head.next;
        for (size_t i = 0; i < len_dst; ++i) {
            assert(cur == dst_nodes[i]);
            cur = cur->next;
        }
        /* post‑condition: original src nodes follow in original order */
        for (size_t i = 0; i < len_src; ++i) {
            assert(cur == src_nodes[i]);
            cur = cur->next;
        }
        /* final node must be the tail sentinel */
        assert(cur == &dst.tail);
    }

    /* unchanged fields of dst's sentinel nodes */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);
    /* unchanged fields of src's sentinel nodes (still empty) */
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);
}
