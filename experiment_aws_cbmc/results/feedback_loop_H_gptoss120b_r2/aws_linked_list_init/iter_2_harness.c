#include <aws/common/linked_list.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* Helper to optionally allocate a single node and link it into the list */
static void maybe_add_one_node(struct aws_linked_list *list, struct aws_allocator *allocator) {
    /* head.prev and tail.next are always NULL for a valid list */
    list->head.prev = NULL;
    list->tail.next = NULL;

    if (nondet_bool()) {
        struct aws_linked_list_node *node = aws_mem_acquire(allocator, sizeof(*node));
        __CPROVER_assume(node != NULL);

        /* link node between head and tail */
        node->prev = &list->head;
        node->next = &list->tail;
        list->head.next = node;
        list->tail.prev = node;
    } else {
        /* empty list */
        list->head.next = &list->tail;
        list->tail.prev = &list->head;
    }
}

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Allocate two lists and optionally populate them with a single node each */
    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, MAX_LINKED_LIST_ITEM_ALLOCATION);

    maybe_add_one_node(&dst, allocator);
    maybe_add_one_node(&src, allocator);

    /* 2. Assume both lists are initially valid */
    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    /* 3. Save old state for later comparison */
    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    bool src_was_empty = aws_linked_list_empty(&old_src);
    bool dst_was_empty = aws_linked_list_empty(&old_dst);

    /* 4. Call the function under test */
    aws_linked_list_move_all_back(&dst, &src);

    /* 5. Post‑condition: both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* 6. src must be empty after the move */
    assert(src.head.next == &src.tail);
    assert(src.tail.prev == &src.head);
    assert(src.head.prev == NULL);
    assert(src.tail.next == NULL);

    /* 7. head.prev and tail.next of dst must still be NULL */
    assert(dst.head.prev == NULL);
    assert(dst.tail.next == NULL);

    if (src_was_empty) {
        /* No nodes moved – dst unchanged */
        assert(dst.head.next == old_dst.head.next);
        assert(dst.tail.prev == old_dst.tail.prev);
    } else {
        /* Nodes were moved from src to the back of dst */

        /* The new last node of dst is the former last node of src */
        assert(dst.tail.prev == old_src.tail.prev);

        if (dst_was_empty) {
            /* dst was empty, so its first node is now the former first node of src */
            assert(dst.head.next == old_src.head.next);
        } else {
            /* dst was non‑empty – its first node stays the same */
            assert(dst.head.next == old_dst.head.next);

            /* The former last node of dst should now point to the former first node of src */
            struct aws_linked_list_node *old_dst_last = old_dst.tail.prev;
            struct aws_linked_list_node *src_first = old_src.head.next;
            assert(old_dst_last->next == src_first);
            assert(src_first->prev == old_dst_last);
        }
    }
}
