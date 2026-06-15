#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    size_t n = nondet_size_t();
    aws_array_list_pop_front_n(&list, n);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (n >= aws_array_list_length(&old)) {
        assert(aws_array_list_length(&list) == 0);
    } else {
        assert(aws_array_list_length(&list) == aws_array_list_length(&old) - n);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
