#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

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
    struct aws_linked_list dst;
    struct aws_linked_list src;
    struct aws_allocator *allocator = aws_default_allocator();

    ensure_linked_list_is_allocated(&dst, allocator, 5);
    ensure_linked_list_is_allocated(&src, allocator, 5);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    struct aws_linked_list_node *src_first_before = src.head->next;
    struct aws_linked_list_node *src_last_before  = src.tail->prev;
    struct aws_linked_list_node *dst_last_before  = dst.tail->prev;

    size_t dst_len_before = count_nodes(&dst);
    size_t src_len_before = count_nodes(&src);

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    if (src_first_before == src.tail) {
        /* src was empty: dst unchanged */
        assert(dst.head->next == old_dst.head->next);
        assert(dst.tail->prev == old_dst.tail->prev);
    } else {
        /* src is now empty */
        assert(src.head->next == src.tail);
        assert(src.tail->prev == src.head);
        assert(src_len_before > 0);

        /* dst now ends with former src nodes */
        assert(dst.tail->prev == src_last_before);
        assert(src_last_before->next == dst.tail);
        assert(src_first_before->prev == dst_last_before);

        if (dst_last_before == dst.head) {
            /* dst was empty */
            assert(dst.head->next == src_first_before);
        } else {
            assert(dst_last_before->next == src_first_before);
        }
    }

    /* Sentinel nodes remain correctly linked */
    assert(dst.head->prev == NULL);
    assert(dst.tail->next == NULL);
    assert(src.head->prev == NULL);
    assert(src.tail->next == NULL);

    /* Length relationship */
    assert(count_nodes(&dst) == dst_len_before + src_len_before);
    assert(count_nodes(&src) == 0);
}
