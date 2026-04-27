// === STEP 1: SUCCESS PATH ===
// When aws_array_list_push_front returns AWS_OP_SUCCESS (0):
//   - list->length: CHANGES to orig_len + 1
//   - list->data: first item_size bytes contain a copy of val
//   - list->current_size: may change (if capacity was expanded)
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_array_list_push_front returns AWS_OP_ERR (-1):
//   - list->length: UNCHANGED
//   - list->data: UNCHANGED (no memmove/memcpy happened)
//   - list->current_size: UNCHANGED
//   - list->item_size: UNCHANGED
//   - list->alloc: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
//   list (struct aws_array_list):
//     - alloc: UNCHANGED always
//     - item_size: UNCHANGED always
//     - length: CHANGED on success (orig_len + 1), UNCHANGED on failure
//     - current_size: CHANGED on success (may grow), UNCHANGED on failure
//     - data: CHANGED on success (contents updated), UNCHANGED on failure
//   val (const void *):
//     - contents: UNCHANGED always (only read)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_array_list_push_front_harness() {
    /* Bound the list to keep verification tractable */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    struct aws_array_list old_list = list;

    /* Create a valid val pointer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(list.item_size > 0);

    size_t orig_len = list.length;

    int result = aws_array_list_push_front(&list, val);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Frame conditions: item_size and alloc never change */
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (result == AWS_OP_SUCCESS) {
        /* length increased by 1 */
        assert(list.length == orig_len + 1);
        /* first item_size bytes of data match val */
        assert(memcmp(list.data, val, list.item_size) == 0);
        /* current_size must be at least enough to hold the new length */
        assert(list.current_size >= list.length * list.item_size);
    } else {
        /* On failure, length and current_size are unchanged */
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
    }
}
