#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_linked_list_init_harness(void) {
    struct aws_linked_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Non‑deterministically decide whether the list is initially non‑empty */
    bool pre_nonempty = nondet_bool();

    if (pre_nonempty) {
        /* Create a dummy node and link it into the list */
        struct aws_linked_list_node *node = malloc(sizeof(*node));
        __CPROVER_assume(node != NULL);

        /* Initialise list head and tail */
        list.head.prev = NULL;
        list.head.next = node;
        list.tail.next = NULL;
        list.tail.prev = node;

        /* Link dummy node */
        node->prev = &list.head;
        node->next = &list.tail;
    } else {
        /* Initialise the list as empty */
        list.head.prev = NULL;
        list.head.next = &list.tail;
        list.tail.prev = &list.head;
        list.tail.next = NULL;
    }

    /* Ensure the allocator field is overwritten by init */
    list.alloc = NULL;

    /* Call the function under test */
    aws_linked_list_init(&list, alloc);

    /* Post‑conditions */
    assert(aws_linked_list_is_valid(&list));
    assert(list.head.next == &list.tail);
    assert(list.tail.prev == &list.head);
    assert(list.alloc == alloc);
}
