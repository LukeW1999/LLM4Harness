#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void aws_linked_list_init_harness(void) {
    struct aws_linked_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    __CPROVER_assume(alloc != NULL);

    aws_linked_list_init(&list, alloc);

    assert(list.head.prev == NULL);
    assert(list.tail.next == NULL);
    assert(list.head.next == &list.tail);
    assert(list.tail.prev == &list.head);
    assert(list.allocator == alloc);
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_empty(&list));
}
