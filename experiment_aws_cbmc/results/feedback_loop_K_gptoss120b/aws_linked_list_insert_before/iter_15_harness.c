#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>

static size_t list_length(const struct aws_linked_list *list) {
    size_t len = 0;
    struct aws_linked_list_node *cur = aws_linked_list_begin(list);
    struct aws_linked_list_node *end = aws_linked_list_end(list);
    while (cur != end) {
        ++len;
        cur = aws_linked_list_next(cur);
    }
    return len;
}

void harness() {
    struct aws_linked_list list;
    aws_linked_list_init(&list);

    struct aws_linked_list_node node_pool[5];
    for (size_t i = 0; i < 5; ++i) {
        aws_linked_list_node_init(&node_pool[i]);
    }

    for (size_t i = 0; i < 5; ++i) {
        if (__CPROVER_nondet_bool()) {
            aws_linked_list_push_back(&list, &node_pool[i]);
        }
    }

    struct aws_linked_list_node *before;
    if (__CPROVER_nondet_bool()) {
        before = &list.head;
    } else {
        size_t idx = __CPROVER_nondet_uint() % 5;
        before = &node_pool[idx];
    }

    struct aws_linked_list_node to_add_node;
    struct aws_linked_list_node *to_add = &to_add_node;
    aws_linked_list_node_init(to_add);

    size_t old_len = list_length(&list);
    struct aws_linked_list_node *old_prev = before->prev;

    aws_linked_list_insert_before(before, to_add);

    assert(aws_linked_list_is_valid(&list));

    size_t new_len = list_length(&list);
    assert(new_len == old_len + 1);

    assert(to_add->next == before);
    assert(to_add->prev == old_prev);
    assert(before->prev == to_add);
}
