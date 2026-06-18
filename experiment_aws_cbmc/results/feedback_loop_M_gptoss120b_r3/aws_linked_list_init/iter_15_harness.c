#include <stddef.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_linked_list_move_all_back_harness(void) {
    struct aws_linked_list src;
    struct aws_linked_list dst;

    aws_linked_list_init(&src);
    aws_linked_list_init(&dst);

    struct aws_linked_list old_dst = dst;

    struct aws_linked_list_node *src_first = src.head.next;
    struct aws_linked_list_node *src_last  = src.tail.prev;
    struct aws_linked_list_node *dst_first = dst.head.next;
    struct aws_linked_list_node *dst_last  = dst.tail.prev;

    aws_linked_list_move_all_back(&dst, &src);

    __CPROVER_assert(aws_linked_list_is_valid(&src), "src is valid");
    __CPROVER_assert(aws_linked_list_is_valid(&dst), "dst is valid");

    __CPROVER_assert(src.head.next == &src.tail, "src head next points to tail");
    __CPROVER_assert(src.tail.prev == &src.head, "src tail prev points to head");
    __CPROVER_assert(src.head.prev == ((void *)0), "src head prev is NULL");
    __CPROVER_assert(src.tail.next == ((void *)0), "src tail next is NULL");

    if (src_first != &src.tail) {
        __CPROVER_assert(dst.tail.prev == src_last, "dst tail prev is src_last");
        __CPROVER_assert(src_last->next == &dst.tail, "src_last next is dst.tail");

        if (dst_first != &dst.tail) {
            __CPROVER_assert(src_first->prev == dst_last, "src_first prev is dst_last");
            __CPROVER_assert(dst_last->next == src_first, "dst_last next is src_first");
        } else {
            __CPROVER_assert(src_first->prev == &dst.head, "src_first prev is dst.head");
            __CPROVER_assert(dst.head.next == src_first, "dst.head next is src_first");
        }
    } else {
        __CPROVER_assert(dst.head.next == old_dst.head.next, "dst.head next unchanged");
        __CPROVER_assert(dst.tail.prev == old_dst.tail.prev, "dst.tail prev unchanged");
        __CPROVER_assert(dst.head.prev == old_dst.head.prev, "dst.head prev unchanged");
        __CPROVER_assert(dst.tail.next == old_dst.tail.next, "dst.tail next unchanged");
    }

    __CPROVER_assert(dst.head.prev == ((void *)0), "dst head prev is NULL");
    __CPROVER_assert(dst.tail.next == ((void *)0), "dst tail next is NULL");
}
