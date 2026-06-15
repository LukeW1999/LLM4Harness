#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_set_at_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a readable source buffer for the value to set */
    size_t item_sz = list.item_size;
    uint8_t *val = NULL;
    if (item_sz > 0) {
        val = malloc(item_sz);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_sz));
    }

    /* 3. Choose a nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Save old state */
    struct aws_array_list old = list;

    /* 5. Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. The list must always remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* Length is increased to index+1 if index was beyond the old length */
        size_t expected_len = (old.length > index) ? old.length : index + 1;
        assert(list.length == expected_len);

        /* item_size and allocator never change */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        /* The stored bytes at the target index must match the source value */
        if (list.data != NULL && item_sz > 0) {
            assert_bytes_match(
                (uint8_t *)list.data + (index * list.item_size),
                val,
                list.item_size);
        }
    } else {
        /* On failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }

    /* 7. Clean up */
    free(val);
}
