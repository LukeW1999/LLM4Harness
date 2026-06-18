#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_init_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    assert(list.head == NULL);
    assert(list.tail == NULL);
}
