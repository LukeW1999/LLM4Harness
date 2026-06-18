#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    uint8_t *old_val = malloc(list.item_size);
    __CPROVER_assume(old_val != NULL);

    size_t index = (size_t)nondet_uint64_t();

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_storage;
    if (old.data != NULL) {
        save_byte_from_array(old.data, old.current_size, &old_storage);
    }

    if (index < old.length) {
        uint8_t *src = (uint8_t *)old.data + (index * old.item_size);
        memcpy(old_val, src, old.item_size);
    }

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        assert(list.data != NULL);
        uint8_t *dest = (uint8_t *)list.data + (index * list.item_size);
        assert_bytes_match(dest, val, list.item_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert_array_list_equivalence(&list, &old, &old_storage);
    }

    assert(aws_array_list_is_valid(&list));

    free(val);
    free(old_val);
}
