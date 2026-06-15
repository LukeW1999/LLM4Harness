#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_end_harness() {
    struct aws_linked_list list;
    
    /* Non-deterministic initialization */
    list.head.next = malloc(sizeof(struct aws_linked_list_node));
    list.tail.prev = malloc(sizeof(struct aws_linked_list_node));
    
    /* Ensure valid list configuration: head.next points to first real node or tail */
    list.head.next->prev = &list.head;
    list.head.next->next = &list.tail;
    list.tail.prev->next = &list.tail;
    list.tail.prev->prev = list.head.next;
    
    /* Make list valid by CBMC's definition */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    
    /* Save old state for comparison */
    struct aws_linked_list old = list;

    /* Call the function under test */
    const struct aws_linked_list_node *rval = aws_linked_list_end(&list);

    /* Postcondition: return value is the address of the tail node */
    assert(rval == &list.tail);

    /* The list must remain unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* Validate list integrity after call */
    assert(aws_linked_list_is_valid(&list));
}
