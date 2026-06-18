#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 64
#define MAX_ITEM_SIZE               32

void aws_array_list_back_harness(void) {
    struct aws_array_list list;
    void *val = NULL;

    /* Allocate an allocator for the list. */
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    /* Structural assumptions. */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Additional safety assumptions. */
    __CPROVER_assume(list.item_size > 0);               /* back requires a non‑zero item size */

    /* Ensure the backing storage is readable for the snapshot. */
    if (list.data != NULL && list.current_size > 0) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(list.data, list.current_size));
    }

    /* Allocate a writable output buffer of the appropriate size. */
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));
    }

    /* Snapshot the pre‑state for frame‑condition checks. */
    struct aws_array_list old_list = list;
    uint8_t *old_data = NULL;
    if (list.data != NULL && list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        __CPROVER_assume(old_data != list.data);      /* avoid overlap for memcpy */
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
