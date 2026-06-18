#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_linked_list_push_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    (void)allocator;

    struct aws_linked_list list;
    aws_linked_list_init(&list);

    size_t pre_len = (size_t)nondet_uint();
    __CPROVER_assume(pre_len <= 5);

    struct aws_linked_list_node existing[5];
    if (pre_len > 0) {
        for (size_t i = 0; i < pre_len; ++i) {
            aws_linked_list_node_reset(&existing[i]);
            aws_linked_list_push_back(&list, &existing[i]);
        }
    }

    struct aws_linked_list_node new_node;
    aws_linked_list_node_reset(&new_node);

    aws_linked_list_push_front(&list, &new_node);

    assert(aws_linked_list_is_valid(&list));

    size_t post_len = 0;
    for (struct aws_linked_list_node *it = list.head.next; it != &list.tail; it = it->next) {
        ++post_len;
    }
    assert(post_len == pre_len + 1);

    assert(list.head.next == &new_node);
    assert(new_node.prev == &list.head);
    assert(new_node.next != NULL);
    if (pre_len == 0) {
        assert(new_node.next == &list.tail);
    } else {
        assert(new_node.next == &existing[0]);
    }

    struct aws_linked_list_node *it = new_node.next;
    for (size_t i = 0; i < pre_len; ++i) {
        assert(it == &existing[i]);
        it = it->next;
    }
    assert(it == &list.tail);

    if (pre_len > 0) {
        assert(existing[0].prev == &new_node);
        for (size_t i = 1; i < pre_len; ++i) {
            assert(existing[i].prev == &existing[i - 1]);
            assert(existing[i].next == (i + 1 == pre_len ? &list.tail : &existing[i + 1]));
        }
    }
}
