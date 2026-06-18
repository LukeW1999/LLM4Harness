#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>

void aws_linked_list_init_harness(void) {
    struct aws_linked_list list;

    aws_linked_list_init(&list);

    __CPROVER_assert(aws_linked_list_is_valid(&list), "list is valid after init");
    __CPROVER_assert(list.head.next == &list.tail, "head next points to tail");
    __CPROVER_assert(list.tail.prev == &list.head, "tail prev points to head");
}
