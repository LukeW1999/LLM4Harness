#include <assert.h>
#include <stdlib.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

int main(void) {
    /* Initialize the linked list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Nondeterministically add up to MAX_LINKED_LIST_ITEM_ALLOCATION items */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_LINKED_LIST_ITEM_ALLOCATION);
    for (size_t i = 0; i < n; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(*node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_push_back(&list, node);
    }

    /* Assume the list is valid after construction */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* Save old state */
    struct aws_linked_list old = list;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_begin(&list);

    /* Postconditions */
    assert(result == old.head.next);
    assert(list.head.next == old.head.next);
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);
    assert(list.tail.prev == old.tail.prev);

    /* Validity invariant */
    assert(aws_linked_list_is_valid(&list));

    return 0;
}
