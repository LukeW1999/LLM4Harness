#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

static struct aws_linked_list_node *nondet_node(void) {
    struct aws_linked_list_node *n = malloc(sizeof(*n));
    __CPROVER_assume(n != NULL);
    n->next = NULL;
    n->prev = NULL;
    return n;
}

void aws_linked_list_pop_back_harness() {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    unsigned int len = __CPROVER_nondet_uint();
    __CPROVER_assume(len <= MAX_LINKED_LIST_ITEM_ALLOCATION);
    for (unsigned int i = 0; i < len; ++i) {
        struct aws_linked_list_node *n = nondet_node();
        aws_linked_list_push_back(&list, n);
    }

    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node *old_tail = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;

    struct aws_linked_list_node *popped = aws_linked_list_pop_back(&list);

    assert(aws_linked_list_is_valid(&list));

    if (len == 0) {
        assert(popped == NULL);
        assert(list.head.next == &list.tail);
        assert(list.tail.prev == &list.head);
    } else {
        assert(popped == old_tail);
        assert(list.tail.prev == old_tail->prev);
        assert(list.head.next == old_head_next);
    }
}
