#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_init_harness(void) {
    struct aws_linked_list list;

    /* Call function under test */
    aws_linked_list_init(&list);

    /* Post‑condition assertions */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.next == &list.tail);
    assert(list.tail.prev == &list.head);
    assert(aws_linked_list_is_valid(&list));
}
