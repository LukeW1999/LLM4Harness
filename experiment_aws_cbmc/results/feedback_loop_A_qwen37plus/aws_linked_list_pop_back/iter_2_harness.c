#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_back_harness(void) {
    struct aws_linked_list list;
    aws_linked_list_init(&list);
    
    __CPROVER_assume(aws_linked_list_is_valid(&list));
    
    if (!aws_linked_list_empty(&list)) {
        struct aws_linked_list_node *node = aws_linked_list_pop_back(&list);
        assert(node != NULL);
        assert(aws_linked_list_is_valid(&list));
    }
}
