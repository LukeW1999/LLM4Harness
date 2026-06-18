#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 256
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 256
#endif

void aws_array_list_swap_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Choose valid indices a and b within bounds */
    size_t length = aws_array_list_length(&list);
    size_t a;
    size_t b;
    __CPROVER_assume(a < length);
    __CPROVER_assume(b < length);

    /* 3. Save old state: list structure and entire data buffer */
    struct aws_array_list old = list;
    /* MAX_BUFFER_SIZE based on maximum initial item allocation and item size */
    uint8_t old_data[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    size_t buf_size = sizeof(old_data);
    __CPROVER_assume(list.current_size <= buf_size);
    memcpy(old_data, list.data, list.current_size);

    /* 4. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 5. Postcondition: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* 6. Unchanged fields (structural invariants) */
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);

    /* 7. Verify that elements at indices a and b are swapped, rest unchanged.
     *    Modify the old_data copy to reflect the expected swap, then compare. */
    uint8_t tmp[MAX_ITEM_SIZE];
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    memcpy(tmp, old_data + a * list.item_size, list.item_size);
    memcpy(old_data + a * list.item_size, old_data + b * list.item_size, list.item_size);
    memcpy(old_data + b * list.item_size, tmp, list.item_size);

    /* Buffer must match expected swapped state */
    assert(memcmp(list.data, old_data, list.current_size) == 0);
}
