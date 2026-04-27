// === STEP 1: SUCCESS PATH ===
// When aws_array_list_front returns AWS_OP_SUCCESS (or the successful value):
//   - list.length: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.data: UNCHANGED
//   - val: CHANGES to the first element of list

// === STEP 2: FAILURE PATH ===
// When aws_array_list_front returns AWS_OP_ERR (or fails):
//   - list.length: UNCHANGED
//   - list.current_size: UNCHANGED
//   - list.item_size: UNCHANGED
//   - list.data: UNCHANGED
//   - val: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// list (struct aws_array_list):
//   - length: UNCHANGED always
//   - current_size: UNCHANGED always
//   - item_size: UNCHANGED always
//   - data: UNCHANGED always
// val (void*):
//   - CHANGES on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/cbmc_utils.h>

void aws_array_list_front_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_ITEM_ALLOC, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    size_t item_size = list.item_size;
    size_t length = list.length;
    size_t current_size = list.current_size;
    void *data = list.data;

    uint8_t val[item_size];
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, item_size));

    struct aws_array_list old_list = list;
    uint8_t old_val[item_size];
    memcpy(old_val, val, item_size);

    int result = aws_array_list_front(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(length > 0);
        assert(memcmp(val, data, item_size) == 0);
        assert(list.length == length);
        assert(list.current_size == current_size);
        assert(list.item_size == item_size);
        assert(list.data == data);
    } else {
        assert(result == aws_raise_error(AWS_ERROR_LIST_EMPTY));
        assert(length == 0);
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(memcmp(val, old_val, item_size) == 0);
    }

    assert(aws_array_list_is_valid(&list));
}
