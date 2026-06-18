#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);
    aws_linked_list_init(list);

    /* nondeterministically create a list with up to 5 nodes */
    size_t num_nodes = nondet_uint();
    __CPROVER_assume(num_nodes <= 5);
    for (size_t i = 0; i < num_nodes; ++i) {
        struct aws_linked_list_node *node = malloc(sizeof(*node));
        __CPROVER_assume(node != NULL);
        aws_linked_list_node_reset(node);
        aws_linked_list_push_back(list, node);
    }

    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* PRE-CALL SNAPSHOT */
    struct aws_linked_list_node *orig_tail_prev = list->tail.prev;
    size_t orig_len = 0;
    for (struct aws_linked_list_node *it = aws_linked_list_begin(list);
         it != aws_linked_list_end(list);
         it = aws_linked_list_next(it)) {
        ++orig_len;
    }

    /* CALL */
    struct aws_linked_list_node *result = aws_linked_list_back(list);

    
}
