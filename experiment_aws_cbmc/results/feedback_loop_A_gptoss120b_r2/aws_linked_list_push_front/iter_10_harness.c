#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list dst;
    struct aws_linked_list src;
    struct aws_allocator *allocator = aws_default_allocator();

    aws_linked_list_init(&dst);
    aws_linked_list_init(&src);

    ensure_linked_list_is_allocated(&dst, allocator, 5);
    ensure_linked_list_is_allocated(&src, allocator, 5);

    __CPROVER_assume(aws_linked_list_is_valid(&dst));
    __CPROVER_assume(aws_linked_list_is_valid(&src));
    __CPROVER_assume(&dst != &src);

    aws_linked_list_move_all_back(&dst, &src);

    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst valid after move");
    __CPROVER_assert(aws_linked_list_is_valid(&src), "src valid after move");
    __CPROVER_assert(aws_linked_list_empty(&src), "src empty after move");
    __CPROVER_assert(src.head.next == &src.tail, "src head next points to tail");
    __CPROVER_assert(src.tail.prev == &src.head, "src tail prev points to head");
    __CPROVER_assert(dst.head.prev == NULL, "dst head prev NULL");
    __CPROVER_assert(dst.tail.next == NULL, "dst tail next NULL");
    __CPROVER_assert(src.head.prev == NULL, "src head prev NULL");
    __CPROVER_assert(src.tail.next == NULL, "src tail next NULL");
}
