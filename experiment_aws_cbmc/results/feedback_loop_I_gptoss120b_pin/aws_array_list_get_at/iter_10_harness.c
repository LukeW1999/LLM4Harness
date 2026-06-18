#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

/* Bounds for nondeterministic allocation sizes */
#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

void aws_array_list_get_at_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    list.allocator = allocator;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Allocate a writable destination buffer for the retrieved value */
    uint8_t val_buf[MAX_ITEM_SIZE];
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = val_buf;
    }

    /* 3. Choose a nondeterministic index */
    size_t index = (size_t)nondet_uint64_t();

    /* 4. Save old state for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    /* 5. Call the function under test */
    int result = aws_array_list_get_at(&list, val, index);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        if (list.item_size > 0) {
            for (size_t i = 0; i < list.item_size; ++i) {
                __CPROVER_assert(
                    ((uint8_t *)val)[i] ==
                    ((uint8_t *)list.data)[index * list.item_size + i],
                    "retrieved element byte mismatch");
            }
        }
        __CPROVER_assert(list.allocator == old.allocator, "allocator changed");
        __CPROVER_assert(list.item_size == old.item_size, "item_size changed");
        __CPROVER_assert(list.length == old.length, "length changed");
        __CPROVER_assert(list.current_size == old.current_size, "current_size changed");
        __CPROVER_assert(list.data == old.data, "data pointer changed");
    } else {
        __CPROVER_assert(list.allocator == old.allocator, "allocator changed on failure");
        __CPROVER_assert(list.item_size == old.item_size, "item_size changed on failure");
        __CPROVER_assert(list.length == old.length, "length changed on failure");
        __CPROVER_assert(list.current_size == old.current_size, "current_size changed on failure");
        __CPROVER_assert(list.data == old.data, "data pointer changed on failure");
        if (old.data != NULL && old.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    /* 7. The array list must always be valid after the call */
    __CPROVER_assert(aws_array_list_is_valid(&list), "list invalid after call");
}
