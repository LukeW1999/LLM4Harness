#include <assert.h>
#include <aws/common/linked_list.h>

void aws_linked_list_rend_harness(void) {
    /* 1. Construct a valid (empty) linked list */
    struct aws_linked_list list;
    list.head.next = &list.tail;
    list.head.prev = &list.tail;
    list.tail.next = &list.head;
    list.tail.prev = &list.head;

    /* 2. Assume the list satisfies the library's validity invariant */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 3. Save old state for later comparison */
    struct aws_linked_list old = list;

    /* 4. Call the function under test */
    const struct aws_linked_list_node *r = aws_linked_list_rend(&list);

    /* 5. Post‑condition checks */
    assert(r == &list.head);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);
    assert(aws_linked_list_is_valid(&list));
}
