#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdlib.h>

#define MAX_INITIAL_ITEM_ALLOCATION 64
#define MAX_ITEM_SIZE               32

void aws_array_list_back_harness(void) {
    struct aws_array_list list;
    void *val = NULL;

    /* Allocate an allocator for the list. */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Choose a nondeterministic, non‑zero item size within limits. */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Initialise a dynamic array list with a bounded capacity. */
    aws_array_list_init_dynamic(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, item_size);

    /* Choose a nondeterministic length that does not exceed the capacity. */
    size_t len;
    __CPROVER_assume(len <= MAX_INITIAL_ITEM_ALLOCATION);
    list.length = len;

    /* Ensure the backing storage is readable/writable for the elements that exist. */
    if (list.data != NULL && len > 0) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(list.data, item_size * len));
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(list.data, item_size * len));
    }

    /* Allocate a writable output buffer of the appropriate size. */
    val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, item_size));

    /* Prevent overlapping between the output buffer and the list's backing storage. */
    if (list.data != NULL && len > 0) {
        __CPROVER_assume(val != list.data);
        __CPROVER_assume(((uint8_t *)val + item_size <= (uint8_t *)list.data) ||
                         ((uint8_t *)list.data + item_size * len <= (uint8_t *)val));
    }

    /* Snapshot the pre‑state for frame‑condition checks. */
    struct aws_array_list old_list = list;
    uint8_t *old_data = NULL;
    if (list.data != NULL && list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(old_data, list.current_size));
        __CPROVER_assume(old_data != list.data);
        __CPROVER_assume(((uint8_t *)old_data + list.current_size <= (uint8_t *)list.data) ||
                         ((uint8_t *)list.data + list.current_size <= (uint8_t *)old_data));
        __builtin_memcpy(old_data, list.data, list.current_size);
    }

    /* Call the function under verification. */
    int result = aws_array_list_back(&list, val);

    /* ---------- POSTCONDITIONS ---------- */

    /* 1. The list must remain valid. */
    assert(aws_array_list_is_valid(&list));

    /* 2. Return‑value correctness. */
    assert((old_list.length > 0) == (result == AWS_OP_SUCCESS));

    /* 3. When the list is non‑empty and the call succeeded, the output buffer must contain the last element. */
    if (old_list.length > 0 && result == AWS_OP_SUCCESS) {
        size_t last_offset = old_list.item_size * (old_list.length - 1);
        assert(__builtin_memcmp(val,
                                (const uint8_t *)old_list.data + last_offset,
                                old_list.item_size) == 0);
    }

    /* 4. Frame conditions – the list structure itself must be unchanged. */
    assert(list.alloc == old_list.alloc);
    assert(list.item_size == old_list.item_size);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.data == old_list.data);

    /* 5. The backing storage must not be modified. */
    if (old_data != NULL) {
        assert(__builtin_memcmp(list.data, old_data, old_list.current_size) == 0);
    }

    return;
}
