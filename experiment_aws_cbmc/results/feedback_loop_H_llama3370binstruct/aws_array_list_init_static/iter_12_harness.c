#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_array_list_init_static_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    void *raw_array = malloc(nondet_size_t());
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(raw_array!= NULL);
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(list.alloc == NULL);
    assert(list.current_size == 0);
    assert(list.item_size == item_size);
    assert(list.length == item_count);
    assert(list.data == raw_array);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
