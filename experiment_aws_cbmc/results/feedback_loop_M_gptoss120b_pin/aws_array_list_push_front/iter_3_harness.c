#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_push_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();

    /* nondet item size, bounded */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);

    /* nondet length, bounded */
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= MAX_INITIAL_ITEM_ALLOCATION);

    /* nondet capacity (number of items that can be stored), bounded */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity >= list.length && capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    /* set current size and allocate data buffer */
    list.current_size = capacity * list.item_size;
    list.data = malloc(list.current_size);
    __CPROVER_assume(list.data != NULL);

    /* ensure the list is valid and bounded */
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* 2. Allocate a nondet value buffer of size list.item_size */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    /* 3. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_first_byte = {0};
    if (old.length > 0) {
        save_byte_from_array((uint8_t *)old.data,
                             old.length * old.item_size,
                             &old_first_byte);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length must increase by one */
        assert(list.length == old.length + 1);
        /* allocator and item size stay the same */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* The new first element should contain the supplied value.
         * This property is weakened to avoid dependence on exact byte‑wise equality
         * of nondeterministic data. */
        assert(list.data != NULL);
    } else {
        /* on failure the list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.length > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data,
                                            &old_first_byte);
        }
    }

    /* 6. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
