Looking at the function:

```c
aws_linked_list_rbegin(const struct aws_linked_list *list) {
    struct aws_linked_list_node *rval = list->tail.prev;
    return rval;
}
```

**Analysis:**
1. **What changes on success?** The function returns `list->tail.prev` - the last element in the list (or head if empty).
2. **What is unchanged?** The list itself is not modified at all - it's a const parameter. All fields remain the same.
3. **Failure path?** No failure path - always returns a pointer.
4. **Validity invariants?** The list must remain valid after the call.

**Postconditions:**
- Return value equals `list->tail.prev`
- The list is unchanged (head, tail pointers all same)
- The list remains valid

```c
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_rbegin_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_linked_list_node *old_head_next = list.head.next;
    struct aws_linked_list_node *old_head_prev = list.head.prev;
    struct aws_linked_list_node *old_tail_next = list.tail.next;
    struct aws_linked_list_node *old_tail_prev = list.tail.prev;

    /* 3. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_rbegin(&list);

    /* 4. Assert postconditions */

    /* Changed fields: return value should be list->tail.prev */
    assert(result == old_tail_prev);

    /* Unchanged fields: list is not modified (const parameter) */
    assert(list.head.next == old_head_next);
    assert(list.head.prev == old_head_prev);
    assert(list.tail.next == old_tail_next);
    assert(list.tail.prev == old_tail_prev);

    /* Validity invariant must hold after the call */
    assert(aws_linked_list_is_valid(&list));

    /* The result pointer must be non-null (tail.prev is always at least head) */
    assert(result != NULL);
}
