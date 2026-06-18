#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_linked_list_pop_back_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_linked_list list;

    aws_linked_list_init(&list);
    ensure_linked_list_is_allocated(&list, allocator, 5);
    __CPROVER_assume(!aws_linked_list_empty(&list));

    aws_linked_list_pop_back(&list);

    assert(aws_linked_list_is_valid(&list));
}
