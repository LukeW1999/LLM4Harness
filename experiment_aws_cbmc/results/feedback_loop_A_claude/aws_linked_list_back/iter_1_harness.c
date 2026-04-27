Looking at the function:

```c
aws_linked_list_back(const struct aws_linked_list *list) {
    struct aws_linked_list_node *rval = list->tail.prev;
    return rval;
}
```

**Analysis:**
1. **What changes on success?** Returns `list->tail.prev` - the last element in the list.
2. **What is unchanged?** The list itself is not modified at all (const parameter).
3. **Failure path?** No failure path - always returns `list->tail.prev`. Precondition: list must not be empty.
4. **Validity invariants?** List must remain valid after the call.

The function requires the list to be non-empty (otherwise `tail.prev` would be `head`, which is not a valid element). The return value should equal `list->tail.prev`.

```c
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_back_harness(void) {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Precondition: list must not be empty (back() requires non-empty list) */
    __CPROVER_assume(!aws_linked_list_empty(&list));

    /* 3. Save old state before calling */
    struct aws_linked_list_node *expected_back = list.tail.prev;
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_back(&list);

    /* 5. Assert postconditions */

    /* Return value must equal tail.prev */
    assert(result == expected_back);
    assert(result == list.tail.prev);

    /* The list must not have been modified (const parameter) */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* Result must not be NULL (list is non-empty, so tail.prev != &head... 
       actually tail.prev should not be the head sentinel) */
    assert(result != NULL);

    /* The returned node must be the one before tail */
    assert(result->next == &list.tail);

    /* 6. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));
}
