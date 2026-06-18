#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_INITIAL_ITEM_ALLOCATION 5
#define MAX_ITEM_SIZE 64

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    list.alloc = allocator;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
        for (size_t i = 0; i < list.item_size; ++i) {
            val[i] = (uint8_t)nondet_uint8_t();
        }
    }

    size_t index = (size_t)nondet_uint64_t();

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_bytes);
    }

    int result = aws_array_list_set_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        if (list.data != NULL && list.item_size > 0) {
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                               val,
                               list.item_size);
        }

        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);

        assert_array_list_equivalence(&list, &old, &old_bytes);
    } else {
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        assert_array_list_equivalence(&list, &old, &old_bytes);
    }

    free(val);
}
