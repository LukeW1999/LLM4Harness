#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_next_harness() {
    /* 1. Allocate a linked list node non-deterministically */
    struct aws_linked_list_node *node = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(node != NULL);

    /* 2. Set up a valid next pointer (can be any valid pointer or NULL) */
    struct aws_linked_list_node *next_node = malloc(sizeof(struct aws_linked_list_node));
    /* next_node can be NULL or a valid pointer — both are valid scenarios */
    node->next = next_node;
    node->prev = malloc(sizeof(struct aws_linked_list_node));

    /* 3. Save old state */
    struct aws_linked_list_node *old_next = node->next;

    /* 4. Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(node);

    /* 5. Assert postconditions */
    /* The function returns node->next */
    assert(result == old_next);
    assert(result == node->next);

    /* 6. Assert node fields are unchanged */
    assert(node->next == old_next);
}
