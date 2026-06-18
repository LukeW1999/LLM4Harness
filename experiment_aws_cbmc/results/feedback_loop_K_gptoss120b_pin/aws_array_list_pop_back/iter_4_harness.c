#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

int aws_array_list_pop_back_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size, must be > 0 */
    size_t item_size = __CPROVER_nondet_size_t();
    __CPROVER_assume(item_size > 0);

    /* nondet capacity (number of items that can fit) */
    size_t capacity = __CPROVER_nondet_size_t();

    /* compute current_size = capacity * item_size, guard overflow */
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(capacity, item_size, &current_size));

    /* nondet length, must be <= capacity */
    size_t length = __CPROVER_nondet_size_t();
    __CPROVER_assume(length <= capacity);

    /* decide whether the list is dynamic (alloc != NULL) or static (alloc == NULL) */
    bool is_dynamic = __CPROVER_nondet_bool();

    if (is_dynamic) {
        list.alloc = alloc;
        if (current_size > 0) {
            list.data = aws_mem_acquire(alloc, current_size);
            __CPROVER_assume(list.data != NULL);
        } else {
            list.data = NULL;
        }
    } else {
        list.alloc = NULL;
        /* static buffer: allocate a concrete buffer on the stack */
        uint8_t static_buf[256];
        __CPROVER_assume(current_size <= sizeof(static_buf));
        list.data = (void *)static_buf;
    }

    list.item_size = item_size;
    list.length = length;
    list.current_size = current_size;

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Preserve a copy of the original state for later comparison */
    struct aws_array_list old = list;
    uint8_t *old_data = NULL;
    if (old.data != NULL && old.current_size > 0) {
        old_data = malloc(old.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, old.data, old.current_size);
    }

    int ret = aws_array_list_pop_back(&list);

    /* Postcondition: list remains valid */
    assert(aws_array_list_is_valid(&list));

    if (old.length > 0) {
        /* Successful pop */
        assert(ret == AWS_OP_SUCCESS);
        assert(list.length == old.length - 1);
        /* allocator, item_size, current_size unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        /* Data buffer pointer unchanged */
        assert(list.data == old.data);
        /* Verify that the removed element bytes are zeroed */
        size_t removed_offset = old.item_size * (old.length - 1);
        uint8_t *removed_ptr = (uint8_t *)list.data + removed_offset;
        for (size_t i = 0; i < old.item_size; ++i) {
            assert(removed_ptr[i] == 0);
        }
        /* Verify that the rest of the buffer is unchanged */
        if (old_data != NULL) {
            for (size_t i = 0; i < removed_offset; ++i) {
                assert(((uint8_t *)list.data)[i] == old_data[i]);
            }
        }
    } else {
        /* Empty list case */
        assert(ret == AWS_ERROR_LIST_EMPTY);
        assert(list.length == 0);
        /* All other fields unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old_data != NULL) {
            for (size_t i = 0; i < old.current_size; ++i) {
                assert(((uint8_t *)list.data)[i] == old_data[i]);
            }
        }
    }

    /* Clean up allocated memory */
    if (is_dynamic && list.data != NULL) {
        aws_mem_release(alloc, list.data);
    }
    if (old_data != NULL) {
        free(old_data);
    }

    return 0;
}
