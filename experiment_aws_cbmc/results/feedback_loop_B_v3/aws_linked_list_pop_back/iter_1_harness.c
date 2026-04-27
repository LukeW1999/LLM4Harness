#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_init_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_linked_list list;

    /* 2. Bound the structure (limits CBMC state space) */
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);

    /* 3. Assume validity precondition (infer from how function uses the struct) */
    /* No preconditions needed for initialization */

    /* 4. Save old state (for checking immutability) */
    struct aws_linked_list old = list;

    /* 5. Assume function-specific preconditions (infer from code logic) */
    /* No specific preconditions needed for initialization */

    /* 6. Call function under test */
    aws_linked_list_init(&list);

    /* 7. Assert postconditions (infer from what the function guarantees) */
    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);
    assert(aws_linked_list_is_valid(&list));
}
