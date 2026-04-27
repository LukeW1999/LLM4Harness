Looking at the function `aws_linked_list_insert_before`:

1. **What fields change on success?**
   - `to_add->next` = `before`
   - `to_add->prev` = `before->prev` (old value)
   - `before->prev->next` = `to_add` (the node before `before` now points forward to `to_add`)
   - `before->prev` = `to_add`

2. **What fields are unchanged?**
   - `before->next` remains unchanged
   - The rest of the list structure beyond these nodes remains unchanged

3. **What happens on failure?**
   - This function has no failure path (void return, no error checking)

4. **What validity invariants must hold after the call?**
   - The list must remain valid (`aws_linked_list_is_valid`)
   - Node linkage must be bidirectional

```c
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_insert_before_harness(void) {
    /* Set up a linked list with some nodes */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* We need a 'before' node that is in the list.
     * Use list.tail (the sentinel tail) as 'before', which is always valid.
     * This simulates inserting before the tail (i.e., push_back scenario).
     * Alternatively, pick a node non-deterministically from the list. */
    struct aws_linked_list_node *before = list.head.next;
    __CPROVER_assume(before != NULL);
    __CPROVER_assume(before->prev != NULL);
    /* before must be a valid node in the list (not the head sentinel) */
    __CPROVER_assume(aws_linked_list_node_next_is_valid(before->prev));
    __CPROVER_assume(aws_linked_list_node_prev_is_valid(before));

    /* The node to insert */
    struct aws_linked_list_node to_add;

    /* Save old state */
    struct aws_linked_list_node *old_before_prev = before->prev;
    struct aws_linked_list_node *old_before_next = before->next;

    /* Call the function under test */
    aws_linked_list_insert_before(before, &to_add);

    /* Postconditions: changed fields */
    assert(to_add.next == before);
    assert(to_add.prev == old_before_prev);
    assert(before->prev == &to_add);
    assert(old_before_prev->next == &to_add);

    /* Postconditions: unchanged fields */
    assert(before->next == old_before_next);

    /* Validity invariants: bidirectional linkage around inserted node */
    assert(aws_linked_list_node_next_is_valid(&to_add));
    assert(aws_linked_list_node_prev_is_valid(&to_add));

    /* The list head and tail sentinels are unchanged */
    assert(aws_linked_list_is_valid(&list));
}
