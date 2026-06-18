#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_front_harness() {
    /* stack-allocated list and node */
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* initialize list */
    aws_linked_list_init(&list);

    /* push a single node onto the back (list is empty, so front == back) */
    aws_linked_list_push_back(&list, &node);

    /* pop front */
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    /* assertions */
    assert(popped == &node);
    assert(aws_linked_list_empty(&list));
    assert(node.next == NULL);
    assert(node.prev == NULL);
}
