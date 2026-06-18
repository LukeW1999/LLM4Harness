#include <aws/common/linked_list.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>

static void maybe_add_one_node(struct aws_linked_list *list, struct aws_allocator *allocator) {
    /* head.prev and tail.next are always NULL for a valid list */
    list->head.prev = NULL;
    list->tail.next = NULL;

    if (nondet_bool()) {
        struct aws_linked_list_node *node = aws_mem_acquire(allocator, sizeof(*node));
        __CPROVER_assume(node != NULL);
        node->prev = &list->head;
        node->next = &list->tail;
        list->head.next = node;
        list->tail.prev = node;
    } else {
        list->head.next = &list->tail;
        list->tail.prev = &list->head;
    }
}

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    struct aws_linked_list src;

    aws_linked_list_init(&dst);
    aws_linked_list_init(&src);

    maybe_add_one_node(&dst, allocator);
    maybe_add_one_node(&src, allocator);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    bool src_was_empty = aws_linked_list_empty(&old_src);
    bool dst_was_empty = aws_linked_list_empty(&old_dst);

    aws_linked_list_move_all_back(&dst, &src);

    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst must be valid");
    __CPROVER_assert(aws_linked_list_is_valid(&src), "src must be valid");

    __CPROVER_assert(src.head.next == &src.tail, "src head next");
    __CPROVER_assert(src.tail.prev == &src.head, "src tail prev");
    __CPROVER_assert(src.head.prev == NULL, "src head prev");
    __CPROVER_assert(src.tail.next == NULL, "src tail next");

    __CPROVER_assert(dst.head.prev == NULL, "dst head prev");
    __CPROVER_assert(dst.tail.next == NULL, "dst tail next");

    if (src_was_empty) {
        __CPROVER_assert(dst.head.next == old_dst.head.next, "dst head next unchanged");
        __CPROVER_assert(dst.tail.prev == old_dst.tail.prev, "dst tail prev unchanged");
    } else {
        __CPROVER_assert(dst.tail.prev == old_src.tail.prev, "dst tail prev from src");
        if (dst_was_empty) {
            __CPROVER_assert(dst.head.next == old_src.head.next, "dst head next from src");
        } else {
            __CPROVER_assert(dst.head.next == old_dst.head.next, "dst head next unchanged");
            struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
            struct aws_linked_list_node *src_first = old_src.head.next;
            __CPROVER_assert(old_dst_last->next == src_first, "link old dst last to src first");
            __CPROVER_assert(src_first->prev == old_dst_last, "link src first back to old dst last");
        }
    }
}
