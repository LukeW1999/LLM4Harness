#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_get_at_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    size_t index = nondet_size_t();
    void *val = aws_array_list_get_at(&list, index);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (val != NULL) {
        assert(index < list.length);
        assert(list.data != NULL);
        assert(list.item_size > 0);
        assert(list.alloc > 0);
    } else {
        assert(index >= list.length);
        assert(list.data != NULL || list.length == 0);
        assert(list.item_size > 0);
        assert(list.alloc > 0);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.length == old.length);
    assert(list.data == old.data);
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
