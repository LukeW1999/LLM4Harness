#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    /* initialize list */
    aws_linked_list_init(&list);

    /* ensure node is not linked */
    node.next = NULL;
    node.prev = NULL;

    /* call the function under test */
    aws_linked_list_push_front(&list, &node);

    /* postconditions */
    assert(list.head.next == &node);
    assert(node.prev == &list.head);
    assert(node.next == &list.tail);
    assert(list.tail.prev == &node);
}
