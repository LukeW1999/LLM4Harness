#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_move_all_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    aws_linked_list_init(&dst);
    __CPROVER_assume(aws_linked_list_is_valid(&dst));

    struct aws_linked_list src;
    aws_linked_list_init(&src);
    __CPROVER_assume(aws_linked_list_is_valid(&src));

    aws_linked_list_move_all_back(&dst, &src);

    assert(aws_linked_list_is_valid(&dst));
    assert(aws_linked_list_is_valid(&src));
}
