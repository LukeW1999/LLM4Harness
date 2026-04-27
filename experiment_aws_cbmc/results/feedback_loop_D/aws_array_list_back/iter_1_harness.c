// === STEP 1: SUCCESS PATH ===
// When aws_array_list_back returns AWS_OP_SUCCESS (or the successful value):
//   - list->data: UNCHANGED
//   - list->current_size: UNCHANGED
//   - list->length: UNCHANGED
//   - list->item_size: UNCHANGED
//   - val: CHANGES to the last element of list->data

// === STEP 2: FAILURE PATH ===
// When aws_array_list_back returns AWS_OP_ERR (or fails):
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
//   - CHANGES on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_list_is_valid(&list): YES (must hold after call)
```

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_array_list_back_harness() {
    struct aws_array_list list;
    void *val;
    size_t max_item_alloc = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    size_t initial_length = nondet_size_t();

    ensure_array_list_has_allocated_data_member(&list);
    list.item_size = max_item_size;
    list.current_size = max_item_alloc * max_item_size;
    list.length = initial_length;
    list.data = malloc(list.current_size);

    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.current_size == list.item_size * max_item_alloc);
    __CPROVER_assume(list.length * list.item_size <= list.current_size);

    val = malloc(max_item_size);

    struct aws_array_list old_list = list;
    void *old_val = val;

    int result = aws_array_list_back(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(memcmp(val, (uint8_t *)list.data + (list.item_size * (list.length - 1)), list.item_size) == 0);
    } else {
        assert(list.alloc == old_list.alloc);
        assert(list.current_size == old_list.current_size);
        assert(list.length == old_list.length);
        assert(list.item_size == old_list.item_size);
        assert(list.data == old_list.data);
        assert(memcmp(val, old_val, list.item_size) == 0);
    }

    assert(aws_array_list_is_valid(&list));

    free(list.data);
    free(val);
}
