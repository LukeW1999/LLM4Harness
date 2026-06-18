/* Preconditions:
 * - list != NULL
 * - either AWS_IS_ZEROED(*list) or aws_array_list_is_valid(list)
 * - if not zeroed, list->alloc may be NULL (static) or non‑NULL (dynamic)
 * - item_size > 0
 * - current_size >= length * item_size
 * - if current_size > 0 then list->data points to writable memory of size current_size
 *
 * Postconditions (validity):
 * - after the call, AWS_IS_ZEROED(*list) || aws_array_list_is_valid(list)
 *
 * Postconditions (length):
 * - if list->data != NULL then list->length == 0
 * - if list->data == NULL then list->length is unchanged
 *
 * Postconditions (frame):
 * - list->alloc, list->item_size, list->current_size, list->data remain unchanged
 * - only list->length may be modified (as above)
 * - the contents of list->data may be overwritten (debug fill) but no other memory is modified
 */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness(void) {
    struct aws_array_list list;
    /* nondet choice: start with a zeroed list or a properly initialized one */
    bool start_zeroed = __CPROVER_nondet_bool();

    if (start_zeroed) {
        /* Zeroed list */
        memset(&list, 0, sizeof(list));
    } else {
        /* Initialize either dynamic or static list */
        bool use_dynamic = __CPROVER_nondet_bool();

        size_t item_size = __CPROVER_nondet_size_t();
        __CPROVER_assume(item_size > 0);

        if (use_dynamic) {
            /* Dynamic allocation */
            size_t init_items = __CPROVER_nondet_size_t();
            /* Prevent overflow in init_items * item_size */
            size_t alloc_size = 0;
            __CPROVER_assume(!aws_mul_size_checked(init_items, item_size, &alloc_size));

            struct aws_allocator *alloc = aws_default_allocator();
            int init_res = aws_array_list_init_dynamic(&list, alloc, init_items, item_size);
            __CPROVER_assume(init_res == AWS_OP_SUCCESS);
        } else {
            /* Static allocation */
            size_t item_count = __CPROVER_nondet_size_t();
            __CPROVER_assume(item_count > 0);
            size_t total_bytes = 0;
            __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &total_bytes));
            void *raw = malloc(total_bytes);
            __CPROVER_assume(raw != NULL);
            aws_array_list_init_static(&list, raw, item_count, item_size);
        }

        /* Set a nondet length that respects capacity */
        size_t capacity = aws_array_list_capacity(&list);
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len <= capacity);
        list.length = len;
        __CPROVER_assume(aws_array_list_is_valid(&list));
    }

    /* Snapshot of fields that must remain unchanged */
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    size_t old_length = list.length; /* may change */

    /* Call the function under test */
    aws_array_list_clear(&list);

    /* Verify post‑conditions */
    assert(AWS_IS_ZEROED(list) || aws_array_list_is_valid(&list));

    if (list.data != NULL) {
        assert(list.length == 0);
    } else {
        assert(list.length == old_length);
    }

    /* Frame conditions */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);
    assert(list.current_size == old_current_size);
    assert(list.data == old_data);

    return 0;
}
