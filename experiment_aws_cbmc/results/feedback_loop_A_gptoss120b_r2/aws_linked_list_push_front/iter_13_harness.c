#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>

void aws_linked_list_push_front_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    struct aws_linked_list_node node;
    node.next = NULL;
    node.prev = NULL;

    aws_linked_list_push_front(&list, &node);

    __CPROVER_assert(!aws_linked_list_empty(&list), "list not empty after push_front");
    __CPROVER_assert(list.head.next == &node, "head next points to node");
    __CPROVER_assert(node.prev == &list.head, "node prev points to head");
    __CPROVER_assert(aws_linked_list_is_valid(&list), "list valid after push_front");
}
