#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_clear_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_clear(&list, aws_default_allocator());

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Changed fields (from Doxygen) */
        assert(list.length == 0);
        assert(list.data != NULL);
        assert(list.current_size >= list.length * list.item_size);

        /* Unchanged fields (implied — Doxygen rarely lists these) */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* Failure: struct unchanged */
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* 7. Test with empty list */
    struct aws_array_list empty_list;
    aws_array_list_init(&empty_list, aws_default_allocator(), 0, sizeof(int));
    aws_array_list_clear(&empty_list, aws_default_allocator());
    assert(aws_array_list_is_valid(&empty_list));
    assert(empty_list.length == 0);
    assert(empty_list.data == NULL || empty_list.current_size == 0);

    /* 8. Test with non-empty list */
    struct aws_array_list non_empty_list;
    aws_array_list_init(&non_empty_list, aws_default_allocator(), 10, sizeof(int));
    for (int i = 0; i < 10; i++) {
        int *item = aws_array_list_push(&non_empty_list);
        *item = i;
    }
    aws_array_list_clear(&non_empty_list, aws_default_allocator());
    assert(aws_array_list_is_valid(&non_empty_list));
    assert(non_empty_list.length == 0);
    assert(non_empty_list.data != NULL);
    assert(non_empty_list.current_size >= non_empty_list.length * non_empty_list.item_size);
}
