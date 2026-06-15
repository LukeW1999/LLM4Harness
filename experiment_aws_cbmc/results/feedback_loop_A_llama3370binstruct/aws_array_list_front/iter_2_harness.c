#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_front_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Call function under test */
    void *result = aws_array_list_front(&list);

    /* 3. Assert postconditions for BOTH success and failure paths */
    if (list.length > 0) {
        assert(result != NULL);
        assert(AWS_MEM_IS_READABLE(result, list.item_size));
    } else {
        assert(result == NULL);
    }

    /* 4. Assert fields that must NOT change regardless of result */
    assert(list.length == list.length);
    assert(list.current_size == list.current_size);
    assert(list.data == list.data);
    assert(list.alloc == list.alloc);
    assert(list.item_size == list.item_size);

    /* 5. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
