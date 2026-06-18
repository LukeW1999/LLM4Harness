#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_ITEM_SIZE 64U
#define MAX_INITIAL_ITEM_ALLOCATION 8U

void aws_array_list_push_front_harness(void) {
    /* 1. Set up a bounded array list */
    struct aws_array_list list;
    list.alloc = aws_default_allocator();

    list.item_size = nondet_uint32_t();
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    list.length = nondet_uint32_t();
    __CPROVER_assume(list.length <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Force a reallocation on push_front */
    list.current_size = list.length * list.item_size;

    /* Allocate the backing buffer */
    ensure_array_list_has_allocated_data_member(&list);

    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* 2. Prepare a readable input value */
    uint8_t *val_buf = malloc(list.item_size);
    __CPROVER_assume(val_buf != NULL);
    for (size_t i = 0; i < list.item_size; ++i) {
        val_buf[i] = nondet_uint8_t();
    }
    const void *val = val_buf;

    /* 3. Save old state */
    struct aws_array_list old = list;

    /* 4. Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* length increased */
        assert(list.length == old.length + 1);

        /* first element matches the supplied value */
        assert_bytes_match((uint8_t *)list.data, val_buf, list.item_size);

        /* previous elements shifted forward */
        if (old.length > 0) {
            for (size_t i = 0; i < old.length * old.item_size; ++i) {
                uint8_t old_byte = ((uint8_t *)old.data)[i];
                uint8_t new_byte = ((uint8_t *)list.data)[list.item_size + i];
                assert(old_byte == new_byte);
            }
        }

        /* allocator and item size unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        /* current_size may have changed due to growth */
    } else {
        /* on failure the list must be unchanged */
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.length > 0) {
            assert_bytes_match((uint8_t *)list.data,
                               (uint8_t *)old.data,
                               old.length * old.item_size);
        }
    }

    /* 6. Invariant: list remains valid */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val_buf);
}
