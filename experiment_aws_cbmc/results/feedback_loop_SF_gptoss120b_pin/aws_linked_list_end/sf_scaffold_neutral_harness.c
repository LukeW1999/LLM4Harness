#include <aws/common/linked_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_end_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);
    __CPROVER_assume(aws_linked_list_is_valid(list));

    /* PRE-CALL SNAPSHOT */
    const struct aws_linked_list_node *pre_tail = &list->tail;

    /* CALL FUNCTION */
    const struct aws_linked_list_node *result = aws_linked_list_end(list);

    
}
