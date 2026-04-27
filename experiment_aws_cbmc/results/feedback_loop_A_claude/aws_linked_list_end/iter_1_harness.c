Looking at the function:

```c
aws_linked_list_end(const struct aws_linked_list *list) {
    const struct aws_linked_list_node *rval = &list->tail;
    return rval;
}
```

**Analysis:**
1. **What changes on success?** Nothing in the list changes. The function returns a pointer to `list->tail`.
2. **What is unchanged?** The entire list structure is unchanged (head, tail, all pointers).
3. **What happens on failure?** No failure path - always returns `&list->tail`.
4. **Validity invariants?** The list must remain valid after the call.

**Postconditions:**
- The return value equals `&list->tail`
- The list is still valid
- No fields changed

```c
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_end_harness() {
    /* 1. Declare and initialize the linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state before calling */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    const struct aws_linked_list_node *result = aws_linked_list_end(&list);

    /* 4. Assert postconditions */

    /* The return value must be a pointer to list->tail */
    assert(result == &list.tail);

    /* The list structure must be unchanged */
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* 5. Assert validity invariant still holds */
    assert(aws_linked_list_is_valid(&list));
}
