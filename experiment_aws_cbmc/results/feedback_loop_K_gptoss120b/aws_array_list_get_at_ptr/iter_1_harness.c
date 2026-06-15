/* Contract for aws_array_list_get_at_ptr
 *
 * Preconditions:
 *   - The pointer 'list' points to a valid aws_array_list object.
 *   - aws_array_list_is_valid(list) holds.
 *   - The pointer 'val' is non‑NULL and points to a writable pointer variable.
 *   - 'index' is an arbitrary size_t value.
 *
 * Postconditions (validity):
 *   - If list->length > index then the function returns AWS_OP_SUCCESS
 *     and *val is set to the address of the element at the given index:
 *       *val == (void *)((uint8_t *)list->data + list->item_size * index)
 *   - Otherwise the function returns a non‑success error code
 *     (AWS_ERROR_INVALID_INDEX) and *val is left unchanged.
 *
 * Postconditions (length):
 *   - list->length is unchanged.
 *
 * Postconditions (frame):
 *   - No fields of the list structure are modified.
 *   - The contents of the underlying data buffer are unchanged.
 *   - No other memory locations are modified.
 */
#include <aws/common/common.h>
#include <aws/common/array_list.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_ptr_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Allocate and initialize list fields */
    list.alloc = alloc;

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 10);               /* bound for tractability */

    list.data = malloc(item_size * capacity);
    __CPROVER_assume(list.data != NULL);

    list.item_size = item_size;
    list.current_size = item_size * capacity;

    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= capacity);

    /* Ensure the list satisfies its internal invariants */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non‑deterministic index */
    size_t index = nondet_size_t();

    /* Prepare the output pointer */
    void *val = NULL;
    void **val_ptr = &val;

    /* Snapshot of the list for frame condition checks */
    struct aws_array_list old = list;
    /* Snapshot of the data buffer */
    uint8_t *old_data = malloc(item_size * capacity);
    __CPROVER_assume(old_data != NULL);
    memcpy(old_data, list.data, item_size * capacity);

    /* Call the function under test */
    int result = aws_array_list_get_at_ptr(&list, val_ptr, index);

    /* Postcondition checks */
    if (list.length > index) {
        assert(result == AWS_OP_SUCCESS);
        assert(val == (void *)((uint8_t *)list.data + list.item_size * index));
    } else {
        assert(result != AWS_OP_SUCCESS);
        assert(val == NULL);
    }

    /* Frame condition checks: list fields unchanged */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);

    /* Data buffer unchanged */
    for (size_t i = 0; i < item_size * capacity; ++i) {
        assert(((uint8_t *)list.data)[i] == old_data[i]);
    }

    free(old_data);
    free(list.data);
    return 0;
}
