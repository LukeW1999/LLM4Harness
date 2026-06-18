#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and initialize the array list */
    struct aws_array_list list;

    /* Use bounded sizes for CBMC tractability */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_count;
    __CPROVER_assume(initial_item_count <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize with static storage to avoid allocation issues */
    size_t data_size;
    __CPROVER_assume(data_size == item_size * initial_item_count || initial_item_count == 0);

    /* Directly set up the list structure */
    list.alloc = allocator;
    list.item_size = item_size;
    list.current_size = item_size * initial_item_count;
    list.length = nondet_size_t();

    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    /* Ensure length is valid: length * item_size <= current_size */
    __CPROVER_assume(list.length <= initial_item_count);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Non-deterministic index */
    size_t index;

    /* 3. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Save a byte from the data for immutability checks on failure */
    struct store_byte_from_buffer old_byte;
    bool saved_byte = false;
    if (list.current_size > 0 && list.data != NULL) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
        saved_byte = true;
    }

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Assert postconditions */

    /* Unchanged fields regardless of outcome */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);
    assert(list.current_size == old_current_size);
    assert(list.data == old_data);

    if (result == AWS_OP_SUCCESS) {
        /* Success: index must have been valid (index < old length) */
        assert(index < old_length);
        /* Length decreases by 1 */
        assert(list.length == old_length - 1);
    } else {
        /* Failure: index was out of bounds */
        assert(index >= old_length);
        /* List is unchanged on failure */
        assert(list.length == old_length);
        /* Data contents unchanged */
        if (saved_byte && list.current_size > 0 && list.data != NULL) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
