#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                                MAX_INITIAL_ITEM_ALLOCATION,
                                                MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val_buf = NULL;
    if (list.item_size > 0) {
        val_buf = malloc(list.item_size);
        __CPROVER_assume(val_buf != NULL);
    }
    const void *val = val_buf;

    size_t index = nondet_size_t();

    struct aws_array_list old = list;

    struct store_byte_from_buffer old_byte;
    bool have_saved_byte = false;
    if (list.data && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
        have_saved_byte = true;
    }

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        size_t expected_length = old.length;
        if (index >= old.length) {
            expected_length = index + 1;
        }
        assert(list.length == expected_length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size >= old.current_size);
        if (list.data) {
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                               (const uint8_t *)val,
                               list.item_size);
        }
        if (have_saved_byte && list.data) {
            size_t modified_start = index * list.item_size;
            size_t modified_end   = modified_start + list.item_size;
            if (old_byte.offset < modified_start || old_byte.offset >= modified_end) {
                assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
            }
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (have_saved_byte && list.data) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    assert(aws_array_list_is_valid(&list));

    free(val_buf);
}
