#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_linked_list_push_front_harness() {
    struct aws_linked_list list;
    struct aws_linked_list_node node;

    __CPROVER_assume(aws_linked_list_is_valid(&list));

    aws_linked_list_push_front(&list, &node);

    assert(aws_linked_list_is_valid(&list));
    assert(!aws_linked_list_empty(&list));
}
