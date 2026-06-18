#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* Declare and initialise the array_list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate and validate the value to be set */
    size_t item_size = list.item_size;
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    /* Choose an index within the bounded range */
    size_t index;
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);

    /* Save old state for immutability checks */
    struct aws_array_list old = list;
    size_t old_len = list.length;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(list.data, list.current_size, &old_byte);

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postcondition 1: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* Postcondition 2: allocator pointer never changes */
    assert(list.alloc == old.alloc);

    /* Postcondition 3: item_size never changes */
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        /* The element at 'index' must equal 'val' */
        uint8_t *dest = (uint8_t *)list.data + (index * item_size);
        assert(AWS_MEM_IS_READABLE(dest, item_size));
        assert(memcmp(dest, val, item_size) == 0);

        /* Length update rule */
        if (index >= old_len) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_len);
        }

        /* All elements except the written one are preserved (if they were valid before) */
        /* The bounded loop is acceptable because MAX_INITIAL_ITEM_ALLOCATION is small */
        size_t i = 0;
        for (; i < old_len; i++) {
            if (i != index) {
                uint8_t *old_elem = (uint8_t *)old.data + (i * item_size);
                uint8_t *new_elem = (uint8_t *)list.data + (i * item_size);
                assert(memcmp(old_elem, new_elem, item_size) == 0);
            }
        }
    } else {
        /* On failure the list must be completely unchanged */
        assert_array_list_equivalence(&list, &old, &old_byte);
        assert(list.length == old_len);
    }

    free(val);
}
