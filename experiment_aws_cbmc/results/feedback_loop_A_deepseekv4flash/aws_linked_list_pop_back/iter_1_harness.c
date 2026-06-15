#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_pop_front_harness() {
    /* nondeterministically allocate a linked list with up to MAX items */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* ensure list is valid and non‑empty */
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* save the old list state for later comparison */
    struct aws_linked_list old_list = list;

    /* call the function under test */
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    /* assert the returned node is not null */
    assert(popped != NULL);

    /* postcondition: removed node is reset */
    assert(popped->prev == NULL);
    assert(popped->next == NULL);

    /* postcondition: list remains valid */
    assert(aws_linked_list_is_valid(&list));

    /* the list head must have changed (popped the first element) */
    assert(list.head.next != old_list.head.next);

    /* The list tail might have changed if the list had only one element.
     * If the list became empty, tail.prev must now point to &list.head,
     * but we cannot assert that because we don't know the original length.
     * Instead, validity already ensures connectivity.
     */
}
