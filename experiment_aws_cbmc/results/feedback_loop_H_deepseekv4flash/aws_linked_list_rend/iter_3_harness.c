#include <aws/common/linked_list.h>
#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"

int main() {
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    assert(aws_linked_list_is_valid(&list));

    const struct aws_linked_list_node *rval = aws_linked_list_rend(&list);

    assert(rval == &list.head);
    assert(rval != NULL);
    assert(aws_linked_list_is_valid(&list));

    return 0;
}
