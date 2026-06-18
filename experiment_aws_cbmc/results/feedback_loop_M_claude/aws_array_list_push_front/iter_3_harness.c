#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 4
#endif

void aws_array_list_push_front_harness() {
    struct aws_array_list list;

    /* Bound the list using the bounded predicate */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* item_size must be positive and bounded */
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);

    /* Allocate val with exactly item_size bytes */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* item_size and alloc must never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length increases by 1 */
        assert(list.length == old_length + 1);
        /* current_size must be >= new length * item_size */
        assert(list.current_size >= list.length * list.item_size);
        /* data must be non-null */
        assert(list.data != NULL);
        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure: length should not have changed */
        assert(list.length == old_length);
        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
