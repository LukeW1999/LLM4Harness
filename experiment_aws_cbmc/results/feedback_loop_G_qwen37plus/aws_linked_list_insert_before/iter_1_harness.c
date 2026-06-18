#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_linked_list_insert_before_harness() {
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    bool is_head = nondet_bool();
    struct aws_linked_list_node *before = is_head ? &list.head : &list.tail;
    
    struct aws_linked_list_node *to_add = malloc(sizeof(struct aws_linked_list_node));
    __CPROVER_assume(to_add != NULL);

    struct aws_linked_list old_list = list;

    aws_linked_list_insert_before(before, to_add);

    if (is_head) {
        assert(to_add->next == &list.head);
        assert(to_add->prev == &list.tail);
        assert(list.head.prev == to_add);
        assert(list.tail.next == to_add);
        
        assert(list.head.next == old_list.head.next);
        assert(list.tail.prev == old_list.tail.prev);
    } else {
        struct aws_linked_list_node *old_prev = old_list.tail.prev;
        assert(to_add->next == &list.tail);
        assert(to_add->prev == old_prev);
        assert(list.tail.prev == to_add);
        assert(old_prev->next == to_add);
        
        assert(list.head.next == old_list.head.next);
        assert(list.head.prev == old_list.head.prev);
        assert(list.tail.next == old_list.tail.next);
    }

    assert(aws_linked_list_is_valid(&list));
}
