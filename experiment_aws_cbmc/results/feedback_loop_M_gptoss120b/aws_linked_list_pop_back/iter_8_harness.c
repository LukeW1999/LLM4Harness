#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    struct aws_linked_list old_dst = dst;
    struct aws_linked_list old_src = src;

    aws_linked_list_move_all_back(&dst, &src);

    /* Both lists must remain valid */
    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));

    /* Source list must be empty */
    assert(src.head->next == src.tail);
    assert(src.tail->prev == src.head);
    assert(src.head->prev == NULL);
    assert(src.tail->next == NULL);

    /* Destination list sentinels must be correctly linked */
    assert(dst.head->prev == NULL);
    assert(dst.tail->next == NULL);

    /* All nodes in the new destination list must come from either the old
       destination or the old source list */
    struct aws_linked_list_node *cur = dst.head->next;
    while (cur != dst.tail) {
        bool in_old_dst = false;
        struct aws_linked_list_node *tmp = old_dst.head->next;
        while (tmp != old_dst.tail) {
            if (cur == tmp) {
                in_old_dst = true;
                break;
            }
            tmp = tmp->next;
        }

        bool in_old_src = false;
        struct aws_linked_list_node *tmp2 = old_src.head->next;
        while (tmp2 != old_src.tail) {
            if (cur == tmp2) {
                in_old_src = true;
                break;
            }
            tmp2 = tmp2->next;
        }

        assert(in_old_dst || in_old_src);
        cur = cur->next;
    }
}
