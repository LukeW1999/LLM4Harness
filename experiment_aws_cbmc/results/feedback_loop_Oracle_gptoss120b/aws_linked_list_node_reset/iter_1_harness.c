#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_node_reset_harness(void) {
    /* Allocate a buffer with guard bytes before and after the node */
    size_t guard_size = 1; /* one byte guard on each side */
    size_t total_size = guard_size + sizeof(struct aws_linked_list_node) + guard_size;
    uint8_t *buf = (uint8_t *)malloc(total_size);
    __CPROVER_assume(buf != NULL);

    uint8_t *pre_guard  = buf;                                   /* byte before the node */
    uint8_t *node_bytes = buf + guard_size;                     /* start of the node */
    uint8_t *post_guard = node_bytes + sizeof(struct aws_linked_list_node); /* byte after the node */

    /* Fill guard bytes with nondeterministic values */
    *pre_guard  = __CPROVER_nondet_uint8_t();
    *post_guard = __CPROVER_nondet_uint8_t();

    /* Create a nondeterministic node */
    struct aws_linked_list_node *node = (struct aws_linked_list_node *)node_bytes;
    node->next = (struct aws_linked_list_node *)__CPROVER_nondet_pointer();
    node->prev = (struct aws_linked_list_node *)__CPROVER_nondet_pointer();

    /* Save copies for later comparison */
    struct aws_linked_list_node old_node = *node;
    uint8_t pre_before  = *pre_guard;
    uint8_t post_before = *post_guard;

    /* Call the function under test */
    aws_linked_list_node_reset(node);

    /* Post‑conditions */

    /* 1. The node must be zeroed */
    assert(node->next == NULL);
    assert(node->prev == NULL);
    assert(AWS_IS_ZEROED(*node));

    /* 2. Guard bytes must be unchanged (frame condition) */
    assert(*pre_guard  == pre_before);
    assert(*post_guard == post_before);

    /* 3. No other memory within the allocated region may have been modified */
    /* (the node itself is allowed to be modified as above) */
    (void)old_node; /* suppress unused variable warning */

    free(buf);
    return 0;
}
