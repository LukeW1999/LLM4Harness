#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Create a pool of nodes on the stack */
    struct aws_linked_list_node nodes[MAX_LINKED_LIST_ITEM_ALLOCATION];

    /* Nondeterministically push a subset of the nodes onto the list */
    for (size_t i = 0; i < MAX_LINKED_LIST_ITEM_ALLOCATION; ++i) {
        __CPROVER_bool include;
        if (include) {
            aws_linked_list_push_back(&list, &nodes[i]);
        }
    }

    /* Preserve the original list state */
    struct aws_linked_list old_list = list;

    /* Record the original length and the last node (if any) */
    size_t old_len = 0;
    struct aws_linked_list_node *old_last = NULL;
    struct aws_linked_list_node *cur = list.head.next;
    while (cur != &list.tail) {
        ++old_len;
        old_last = cur;
        cur = cur->next;
    }

    /* Perform the operation under test */
    aws_linked_list_pop_back(&list);

    /* The list must remain valid */
    assert(aws_linked_list_is_valid(&list));

    /* Compute the new length */
    size_t new_len = 0;
    cur = list.head.next;
    while (cur != &list.tail) {
        ++new_len;
        cur = cur->next;
    }

    /* Length should decrease by one if the list was non‑empty */
    if (old_len > 0) {
        assert(new_len == old_len - 1);
    } else {
        assert(new_len == 0);
    }

    /* All remaining nodes must be from the original list and not the removed node */
    cur = list.head.next;
    while (cur != &list.tail) {
        bool found = false;
        struct aws_linked_list_node *tmp = old_list.head.next;
        while (tmp != &old_list.tail) {
            if (cur == tmp) {
                found = true;
                break;
            }
            tmp = tmp->next;
        }
        assert(found);
        if (old_last != NULL) {
            assert(cur != old_last);
        }
        cur = cur->next;
    }
}
