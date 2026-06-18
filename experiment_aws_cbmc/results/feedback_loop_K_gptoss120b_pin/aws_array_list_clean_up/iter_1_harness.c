/* Contract:
   Preconditions:
     - `list` is a non‑NULL pointer to a `struct aws_array_list`.
     - If `list->alloc` is non‑NULL, it points to a valid `struct aws_allocator`.
     - If `list->data` is non‑NULL, it points to a memory region that was allocated
       by `list->alloc` (or by the default allocator when `list->alloc` is NULL).
   Postconditions (validity):
     - After the call, all fields of `*list` are zero (i.e., `list->alloc`,
       `list->data`, `list->current_size`, `list->length`, `list->item_size` are all 0/NULL).
   Postconditions (memory):
     - If `list->alloc` and `list->data` were both non‑NULL on entry, the memory
       pointed to by `list->data` has been released via `aws_mem_release`.
   Postconditions (frame):
     - No memory locations outside of `list` and the memory region originally
       pointed to by `list->data` are modified. */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/array_list.h>
#include <aws/common/memory.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clean_up_harness(void) {
    /* Allocate the list structure */
    struct aws_array_list *list = malloc(sizeof(*list));
    __CPROVER_assume(list != NULL);

    /* Non‑deterministically decide whether the list has an allocator */
    bool has_alloc = __CPROVER_nondet_bool();
    if (has_alloc) {
        list->alloc = aws_default_allocator();
    } else {
        list->alloc = NULL;
    }

    /* Non‑deterministically decide whether the list has data */
    bool has_data = __CPROVER_nondet_bool();
    if (has_data) {
        size_t data_size = __CPROVER_nondet_uint();
        __CPROVER_assume(data_size > 0);
        if (list->alloc) {
            list->data = aws_mem_acquire(list->alloc, data_size);
        } else {
            /* If no allocator, fall back to malloc for the purpose of the harness */
            list->data = malloc(data_size);
        }
        __CPROVER_assume(list->data != NULL);
    } else {
        list->data = NULL;
    }

    /* Initialize the remaining fields with nondeterministic values */
    list->current_size = __CPROVER_nondet_uint();
    list->length = __CPROVER_nondet_uint();
    list->item_size = __CPROVER_nondet_uint();

    /* Allocate a separate buffer to check the frame condition */
    unsigned char *frame_buffer = malloc(16);
    __CPROVER_assume(frame_buffer != NULL);
    for (size_t i = 0; i < 16; ++i) {
        frame_buffer[i] = __CPROVER_nondet_uchar();
    }
    /* Save a copy of the frame buffer */
    unsigned char frame_buffer_copy[16];
    for (size_t i = 0; i < 16; ++i) {
        frame_buffer_copy[i] = frame_buffer[i];
    }

    /* Call the function under verification */
    aws_array_list_clean_up(list);

    /* Post‑condition checks for the list fields */
    assert(list->alloc == NULL);
    assert(list->data == NULL);
    assert(list->current_size == 0);
    assert(list->length == 0);
    assert(list->item_size == 0);

    /* Frame condition: the unrelated buffer must be unchanged */
    for (size_t i = 0; i < 16; ++i) {
        assert(frame_buffer[i] == frame_buffer_copy[i]);
    }

    /* Clean up the harness allocations */
    free(frame_buffer);
    free(list);
    return 0;
}
