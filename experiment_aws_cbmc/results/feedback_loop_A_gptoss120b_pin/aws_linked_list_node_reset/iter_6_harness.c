#include <assert.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

/* Helper to count nodes in a list */
static size_t count_nodes(const struct aws_linked_list *list) {
    size_t cnt = 0;
    if (list->head == NULL) {
        return 0;
    }
    for (struct aws_linked_list_node *cur = list->head; cur != NULL; cur = cur->next) {
        ++cnt;
    }
    return cnt;
}

void aws_linked_list_move_all_back_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    aws_linked_list_init(&dst);
    aws_linked_list_init(&src);

    size_t len_dst;
    size_t len_src;
    __CPROVER_assume(len_dst <= MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(len_src <= MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list_node dst_nodes_storage[MAX_LINKED_LIST_ITEM_ALLOCATION];
    struct aws_linked_list_node *dst_nodes[MAX_LINKED_LIST_ITEM_ALLOCATION];
    for (size_t i = 0; i < len_dst; ++i) {
        dst_nodes[i] = &dst_nodes_storage[i];
        aws_linked_list_push_back(&dst, dst_nodes[i]);
    }

    struct aws_linked_list_node src_nodes_storage[MAX_LINKED_LIST_ITEM_ALLOCATION];
    struct aws_linked_list_node *src_nodes[MAX_LINKED_LIST_ITEM_ALLOCATION];
    for (size_t i = 0; i < len_src; ++i) {
        src_nodes[i] = &src_nodes_storage[i];
        aws_linked_list_push_back(&src, src_nodes[i]);
    }

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* src should be empty after the move */
    assert(src.head == NULL);
    assert(src.tail == NULL);

    assert(count_nodes(&dst) == len_dst + len_src);

    {
        struct aws_linked_list_node *cur = dst.head;
        for (size_t i = 0; i < len_dst; ++i) {
            assert(cur == dst_nodes[i]);
            cur = cur->next;
        }
        for (size_t i = 0; i < len_src; ++i) {
            assert(cur == src_nodes[i]);
            cur = cur->next;
        }
        assert(cur == NULL);
    }

    if (dst.head != NULL) {
        assert(dst.head->prev == NULL);
    }
    if (dst.tail != NULL) {
        assert(dst.tail->next == NULL);
    }
}
