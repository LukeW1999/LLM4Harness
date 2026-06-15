#include <aws/common/linked_list.h>

void aws_linked_list_init_harness() {
    struct aws_linked_list list;

    /* Precondition: list is non-null (satisfied by local variable) */
    __CPROVER_assume(&list != NULL);

    aws_linked_list_init(&list);

    /* Postcondition: list points to itself in a circular manner */
    assert(list.head.next == &list.tail);
    assert(list.tail.prev == &list.head);

    /* Additional postcondition: orphaned pointers are null */
    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
}
