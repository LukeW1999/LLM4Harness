#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/cbmc_utils.h>

void aws_array_list_set_at_harness() {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.length > 0);
    __CPROVER_assume(list.length <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(list.current_size == list.length * list.item_size);
    __CPROVER_assume(list.data != NULL);
    __CPROVER_assume(list.alloc == allocator);

    /* value to set */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* nondet index, allow both success and failure paths */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= list.length + 5); /* give some room for out‑of‑bounds */

    /* snapshot old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    /* call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* post‑condition: list must remain valid */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* success: index must be within bounds */
        assert(index < old.length);
        assert_bytes_match(
            (uint8_t *)list.data + (index * list.item_size),
            val,
            list.item_size);
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        if (list.data == old.data && old.data != NULL && old.current_size > 0) {
            size_t total = old.length * old.item_size;
            size_t prefix = index * old.item_size;
            if (prefix > 0) {
                assert_bytes_match((uint8_t *)list.data,
                                   (uint8_t *)old.data,
                                   prefix);
            }
            size_t suffix_start = (index + 1) * old.item_size;
            if (suffix_start < total) {
                size_t suffix_len = total - suffix_start;
                assert_bytes_match((uint8_t *)list.data + suffix_start,
                                   (uint8_t *)old.data + suffix_start,
                                   suffix_len);
            }
        }
    } else {
        /* failure: index out of bounds */
        assert(index >= old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data != NULL && old.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    free(val);
}
