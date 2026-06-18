#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

/* Helper to count nodes – used only for optional deeper reasoning */
static size_t count_nodes(const struct aws_linked_list *list) {
    size_t cnt = 0;
    struct aws_linked_list_node *cur = list->head->next;
    while (cur != list->tail) {
        cnt++;
        cur = cur->next;
    }
    return cnt;
}

void aws_linked_list_move_all_back_harness(void) {
    /* 1. Allocate and bound the two lists */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    struct aws_allocator *allocator = aws_default_allocator();

    ensure_linked_list_is_allocated(&dst, allocator, 5);
    ensure_linked_list_is_allocated(&src, allocator, 5);

    /* 2. Assume preconditions */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);               /* dst and src are distinct objects */

    /* 3. Save old state */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *src_first_before = src.head->next;
    struct aws_linked_list_node *src_last_before  = src.tail->prev;
    struct aws_linked_list_node *dst_last_before  = dst.tail->prev;

    size_t dst_len_before = count_nodes(&dst);
    size_t src_len_before = count_nodes(&src);

    /* 4. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 5. Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 6. If src was empty, dst must be unchanged */
    if (src_first_before == src.tail) { /* src was empty */
        assert(dst.head->next == old_dst.head->next);
        assert(dst.tail->prev == old_dst.tail->prev);
    } else {
        /* src is now empty */
        assert(src.head->next == src.tail);
        assert(src.tail->prev == src.head);
        assert(src_len_before > 0);

        /* dst now contains its old nodes followed by the former src nodes */
        /* New tail predecessor must be the former last node of src */
        assert(dst.tail->prev == src_last_before);

        /* The first node taken from src must be linked after the old last node of dst */
        if (dst_last_before == dst.head) { /* dst was empty */
            assert(dst.head->next == src_first_before);
        } else {
            assert(dst.head->next == old_dst.head->next);
            assert(dst_last_before->next == src_first_before);
        }
        assert(src_first_before->prev == dst_last_before);
        assert(src_last_before->next == dst.tail);
    }

    /* 7. Unchanged fields of the list structures (head/tail nodes themselves) */
    assert(dst.head->prev == NULL);
    assert(dst.tail->next == NULL);
    assert(src.head->prev == NULL);
    assert(src.tail->next == NULL);

    /* 8. Length relationship post‑condition */
    assert(count_nodes(&dst) == dst_len_before + src_len_before);
    assert(count_nodes(&src) == 0);
}
