#include <assert.h>
#include <stdlib.h>
#include <aws/common/linked_list.h>

void aws_linked_list_rend_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* optionally add a single element to the list */
    if (__CPROVER_nondet_bool()) {
        struct aws_linked_list_node *node = malloc(sizeof(*node));
        __CPROVER_assume(node != NULL);
        node->next = NULL;
        node->prev = NULL;
        aws_linked_list_push_back(&list, node);
    }

    /* save a copy of the list state */
    struct aws_linked_list old = list;

    /* call the function under test */
    const struct aws_linked_list_node *r = aws_linked_list_rend(&list);

    /* post‑condition checks */
    assert(r == &list.head);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}
