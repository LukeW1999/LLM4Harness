#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* nondet helpers */
extern unsigned int nondet_uint(void);
extern void *nondet_ptr(void);

void aws_linked_list_pop_front_harness(void) {
    /* allocator (not used directly but required by some helpers) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Create and initialize a list */
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    /* Build a non‑empty list with a nondet number of nodes (bounded) */
    unsigned int num_nodes = nondet_uint();
    __CPROVER_assume(num_nodes > 0 && num_nodes <= 5);

    for (unsigned int i = 0; i < num_nodes; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof *node);
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        aws_linked_list_push_back(&list, node);
    }

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* PRE‑CALL SNAPSHOT */
    size_t original_len = 0;
    struct aws_linked_list_node *original_front = NULL;
    struct aws_linked_list_node *it = list.head.next;
    while (it != &list.tail) {
        if (original_len == 0) {
            original_front = it;
        }
        ++original_len;
        it = it->next;
    }

    /* Call the function under verification */
    struct aws_linked_list_node *popped = aws_linked_list_pop_front(&list);

    /* ASSERT_POSTCONDITIONS_HERE */
}
