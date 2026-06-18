#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_move_all_back_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list dst;
    struct aws_linked_list src;

    ensure_linked_list_is_allocated(&dst, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);
    ensure_linked_list_is_allocated(&src, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    aws_linked_list_move_all_back(&dst, &src);

    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst remains valid");
    __CPROVER_assert(aws_linked_list_is_valid(&src), "src remains valid");

    __CPROVER_assert(src.head.next == &src.tail, "src head points to tail");
    __CPROVER_assert(src.tail.prev == &src.head, "src tail points to head");

    __CPROVER_assert(dst.head.prev == NULL, "dst head.prev is NULL");
    __CPROVER_assert(dst.tail.next == NULL, "dst tail.next is NULL");
    __CPROVER_assert(src.head.prev == NULL, "src head.prev is NULL");
    __CPROVER_assert(src.tail.next == NULL, "src tail.next is NULL");
}
