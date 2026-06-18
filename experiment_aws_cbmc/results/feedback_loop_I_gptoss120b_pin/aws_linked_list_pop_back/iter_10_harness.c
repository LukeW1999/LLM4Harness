#define AWS_STATIC_IMPL
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/linked_list.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LINKED_LIST_ITEM_ALLOCATION 5

void aws_linked_list_pop_back_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_linked_list list;
    aws_linked_list_init(&list);
    ensure_linked_list_is_allocated(&list, allocator, MAX_LINKED_LIST_ITEM_ALLOCATION);

    struct aws_linked_list_node *old_head = list.head;
    struct aws_linked_list_node *old_tail = list.tail;

    struct aws_linked_list_node *out = NULL;
    bool result = aws_linked_list_pop_back(&list, &out);

    assert(aws_linked_list_is_valid(&list));

    if (old_tail == NULL) {
        assert(result == false);
        assert(out == NULL);
        assert(list.head == NULL);
        assert(list.tail == NULL);
    } else {
        assert(result == true);
        assert(out == old_tail);
        struct aws_linked_list_node *new_tail = old_tail->prev;
        assert(list.tail == new_tail);
        if (new_tail != NULL) {
            assert(new_tail->next == NULL);
            assert(list.head == old_head);
        } else {
            assert(list.head == NULL);
        }
    }
}
