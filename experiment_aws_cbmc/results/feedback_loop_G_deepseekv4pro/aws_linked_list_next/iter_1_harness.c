#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_next_harness() {
    struct aws_linked_list_node node;
    /* Initialize fields nondeterministically */
    node.next = (struct aws_linked_list_node *)(size_t)nondet_size_t();
    node.prev = (struct aws_linked_list_node *)(size_t)nondet_size_t();

    /* Save old state */
    struct aws_linked_list_node old_node = node;

    /* Call function under test */
    struct aws_linked_list_node *result = aws_linked_list_next(&node);

    /* Assert return value equals old next pointer */
    assert(result == old_node.next);

    /* Assert node fields are unchanged */
    assert(node.next == old_node.next);
    assert(node.prev == old_node.prev);
}
