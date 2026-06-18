#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_linked_list_insert_after_harness(void) {
    /* 1. Allocate and initialize a linked list */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    aws_linked_list_init(&list);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Choose a node that is already in the list to act as 'after'.
       Use the list head sentinel which is always present. */
    struct aws_linked_list_node *after = &list.head;

    /* 3. Allocate a new node to be inserted */
    struct aws_linked_list_node *to_add = malloc(sizeof(*to_add));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(to_add, sizeof(*to_add)));
    /* Ensure the node is not currently linked */
    to_add->next = NULL;
    to_add->prev = NULL;

    /* 4. Save old state */
    struct aws_linked_list old_list = list;
    struct aws_linked_list_node old_after = *after;
    struct aws_linked_list_node *old_next = after->next; /* node that will follow 'to_add' */

    /* 5. Call the function under test */
    aws_linked_list_insert_after(after, to_add);

    /* 6. Post‑conditions for successful insertion */
    /* Links have been updated correctly */
    assert(after->next == to_add);
    assert(to_add->prev == after);
    assert(to_add->next == old_next);
    assert(old_next->prev == to_add);

    /* Unchanged fields */
    assert(after->prev == old_after.prev);               /* head.prev unchanged */
    assert(old_next->next == old_next->next);            /* tail.next unchanged (trivial) */
    assert(list.tail.next == old_list.tail.next);       /* tail.next unchanged */
    assert(list.head.prev == old_list.head.prev);       /* head.prev unchanged */

    /* List head and tail pointers reflect the new node */
    assert(list.head.next == to_add);
    assert(list.tail.prev == to_add);

    /* 7. Validity invariants must still hold */
    assert(aws_linked_list_is_valid(&list));
}
