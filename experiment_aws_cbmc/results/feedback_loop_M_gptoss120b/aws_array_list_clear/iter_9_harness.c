#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

/* Nondeterministic value generators provided by CBMC */
extern uint64_t nondet_uint64_t(void);
extern uint8_t nondet_uint8_t(void);

#define MAX_INITIAL_ITEM_ALLOCATION 5
#define MAX_ITEM_SIZE 64

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Allocate and initialize the array list */
    struct aws_array_list list;
    aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);

    /* 2. Nondeterministically set the logical length */
    __CPROVER_assume(list.length <= MAX_INITIAL_ITEM_ALLOCATION);
    list.current_size = list.length * list.item_size;

    /* 3. Save old state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    }

    /* 4. Prepare nondeterministic input value */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        for (size_t i = 0; i < list.item_size; ++i) {
            val[i] = nondet_uint8_t();
        }
    }

    /* 5. Nondeterministic index, bounded by capacity */
    size_t index = (size_t)(nondet_uint64_t() % (MAX_INITIAL_ITEM_ALLOCATION + 1));

    /* 6. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 7. Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));

        if (list.item_size > 0 && index < list.length) {
            uint8_t *dst = (uint8_t *)list.data + index * list.item_size;
            for (size_t i = 0; i < list.item_size; ++i) {
                assert(dst[i] == val[i]);
            }
        }

        size_t expected_len = old.length;
        if (index >= old.length) {
            expected_len = index + 1;
        }
        assert(list.length == expected_len);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        if (list.current_size > 0) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
        assert(aws_array_list_is_valid(&list));
    }

    assert(aws_array_list_is_valid(&list));
    free(val);
    aws_array_list_clean_up(&list);
}
