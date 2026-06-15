#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_pop_back_harness() {
    /* 1. Declare and bound data structures */
    struct aws_linked_list list;
    ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
    __CPROVER_assume(aws_linked_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_linked_list old = list;

    /* 3. Call function under test */
    bool success = aws_linked_list_pop_back(&list, aws_default_allocator());

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(aws_linked_list_is_valid(&list));
    if (success) {
        assert(aws_linked_list_empty(&list) || list.head!= list.tail);
    } else {
        assert(aws_linked_list_empty(&old));
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.head.prev == old.head.prev);
    assert(list.tail.next == old.tail.next);

    /* 6. Assert validity invariant always holds */
    assert(aws_linked_list_is_valid(&list));

    /* 7. Test edge cases */
    if (aws_linked_list_empty(&old)) {
        assert(!success);
    } else {
        assert(success);
    }
}

int main() {
    aws_linked_list_pop_back_harness();
    return 0;
}
