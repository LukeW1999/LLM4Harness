// === STEP 1: SUCCESS PATH ===
// When aws_array_list_back returns AWS_OP_SUCCESS:
//   - list->data: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->length: UNCHANGED
//   - list->item_size: UNCHANGED
//   - val: CHANGED to the last element of list

// === STEP 2: FAILURE PATH ===
// When aws_array_list_back returns AWS_OP_ERR:
//   - list->data: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->length: UNCHANGED
//   - list->item_size: UNCHANGED
//   - val: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - alloc: UNCHANGED always
//   - current_size: UNCHANGED always
//   - length: UNCHANGED always
//   - item_size: UNCHANGED always
//   - data: UNCHANGED always
// val (void*):
//   - CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_back_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_ITEM_ALLOCATION_or_0, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    size_t old_length = list.length;

    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    struct aws_array_list old_list = list;
    void *old_val = malloc(list.item_size);
    memcpy(old_val, val, list.item_size);

    int result = aws_array_list_back(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.data == old_list.data);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        if (old_length > 0) {
            size_t last_item_offset = old_list.item_size * (old_length - 1);
            assert_bytes_match(val, (uint8_t *)old_list.data + last_item_offset, old_list.item_size);
        }
    } else {
        assert(list.data == old_list.data);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert_bytes_match(val, old_val, old_list.item_size);
    }

    assert(aws_array_list_is_valid(&list));
}
