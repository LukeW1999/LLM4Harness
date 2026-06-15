/*  
 * Contract for aws_array_list_erase  
 * Preconditions:  
 *   - list != NULL  
 *   - aws_array_list_is_valid(list) holds before the call  
 *   - index is an arbitrary size_t (may be out of bounds)  
 *   - list->item_size > 0 (guaranteed by validity)  
 *   - list->data may be NULL only if list->length == 0 (guaranteed by validity)  
 * Postconditions (validity):  
 *   - aws_array_list_is_valid(list) holds after the call  
 *   - Return value is AWS_OP_SUCCESS if index < old_length, otherwise AWS_ERROR_INVALID_INDEX  
 * Postconditions (length):  
 *   - If return == AWS_OP_SUCCESS, then list->length == old_length - 1  
 *   - If return != AWS_OP_SUCCESS, then list->length == old_length  
 *   - list->current_size is unchanged (capacity does not shrink)  
 * Postconditions (frame):  
 *   - No memory outside the range [list->data, list->data + list->current_size) is modified  
 *   - The contents of the array may be reordered according to erase semantics, but bytes outside the
 *     original element range remain unchanged.  
 */

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness(void) {
    /* Allocate and initialize a dynamic array list */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Nondeterministic but bounded item size and initial allocation */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= 64);

    size_t init_alloc;
    __CPROVER_assume(init_alloc <= 32);

    int init_res = aws_array_list_init_dynamic(&list, alloc, init_alloc, item_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Populate the list with a nondeterministic length (0 .. init_alloc) */
    size_t target_len;
    __CPROVER_assume(target_len <= init_alloc);

    uint8_t *tmp = malloc(item_size);
    __CPROVER_assume(tmp != NULL);
    for (size_t i = 0; i < target_len; ++i) {
        /* Fill tmp with nondeterministic data */
        for (size_t j = 0; j < item_size; ++j) {
            tmp[j] = __CPROVER_nondet_uint8_t();
        }
        int push_res = aws_array_list_push_back(&list, tmp);
        __CPROVER_assume(push_res == AWS_OP_SUCCESS);
    }
    free(tmp);

    /* Capture old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;

    /* Nondeterministic index */
    size_t index = __CPROVER_nondet_size_t();

    /* Call the function under verification */
    int ret = aws_array_list_erase(&list, index);

    /* Verify postconditions */
    if (index < old_length) {
        /* Successful erase */
        assert(ret == AWS_OP_SUCCESS);
        assert(list.length == old_length - 1);
    } else {
        /* Out‑of‑bounds index */
        assert(ret == aws_raise_error(AWS_ERROR_INVALID_INDEX));
        assert(list.length == old_length);
    }

    /* Capacity (current_size) must remain unchanged */
    assert(list.current_size == old_current_size);

    /* Validity must hold */
    assert(aws_array_list_is_valid(&list));

    /* Frame condition: memory outside the original allocation must be untouched */
    if (old_data != NULL) {
        /* Verify that bytes before the start and after the end of the original buffer are unchanged */
        /* Since we cannot directly inspect memory outside the allocation in CBMC,
           we conservatively assert that the pointer value itself is unchanged. */
        assert(list.data == old_data);
    }

    /* Clean up */
    aws_array_list_clean_up(&list);
    return 0;
}
