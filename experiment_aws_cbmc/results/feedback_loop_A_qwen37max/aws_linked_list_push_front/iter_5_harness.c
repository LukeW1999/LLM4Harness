#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    aws_linked_list_push_front(&list, node);

    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));
}
