#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_push_back_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    bool is_dynamic = __CPROVER_nondet_bool();

    if (is_dynamic) {
        size_t initial_item_allocation = __CPROVER_nondet_size_t();
        size_t item_size = __CPROVER_nondet_size_t();

        __CPROVER_assume(item_size > 0);
        __CPROVER_assume(initial_item_allocation <= 1024);
        __CPROVER_assume(
            aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size) == AWS_OP_SUCCESS);
    } else {
        size_t item_count = __CPROVER_nondet_size_t();
        size_t item_size = __CPROVER_nondet_size_t();

        __CPROVER_assume(item_count > 0);
        __CPROVER_assume(item_size > 0);
        __CPROVER_assume(item_count <= 1024);
        void *raw = malloc(item_count * item_size);
        __CPROVER_assume(raw != NULL);
        __CPROVER_assume(
            aws_array_list_init_static(&list, raw, item_count, item_size) == AWS_OP_SUCCESS);
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    size_t old_length = aws_array_list_length(&list);
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    uint8_t *old_contents = NULL;
    if (old_data != NULL && old_current_size > 0) {
        old_contents = malloc(old_current_size);
        __CPROVER_assume(old_contents != NULL);
        for (size_t i = 0; i < old_current_size; ++i) {
            old_contents[i] = ((uint8_t *)old_data)[i];
        }
    }

    int result = aws_array_list_push_back(&list, val);

    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR,
                     "result must be success or error");

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(aws_array_list_is_valid(&list), "list must be valid after success");
        __CPROVER_assert(aws_array_list_length(&list) == old_length + 1,
                         "length increased by one");
        __CPROVER_assert(list.item_size == old_item_size,
                         "item size unchanged");
        __CPROVER_assert(list.current_size >= (old_length + 1) * list.item_size,
                         "capacity sufficient for new length");

        if (old_contents != NULL && list.data != NULL) {
            size_t bytes = old_length * list.item_size;
            for (size_t i = 0; i < bytes; ++i) {
                __CPROVER_assert(old_contents[i] == ((uint8_t *)list.data)[i],
                                 "existing element unchanged");
            }
        }

        if (list.data != NULL) {
            uint8_t *dest = (uint8_t *)list.data + old_length * list.item_size;
            uint8_t *src = (uint8_t *)val;
            for (size_t i = 0; i < list.item_size; ++i) {
                __CPROVER_assert(dest[i] == src[i],
                                 "new element stored correctly");
            }
        }
    } else {
        __CPROVER_assert(aws_array_list_is_valid(&list), "list must remain valid after failure");
        __CPROVER_assert(aws_array_list_length(&list) == old_length,
                         "length unchanged after failure");
        __CPROVER_assert(list.current_size == old_current_size,
                         "current_size unchanged after failure");
        __CPROVER_assert(list.item_size == old_item_size,
                         "item_size unchanged after failure");
        __CPROVER_assert(list.data == old_data,
                         "data pointer unchanged after failure");
        if (old_contents != NULL && old_data != NULL) {
            for (size_t i = 0; i < old_current_size; ++i) {
                __CPROVER_assert(old_contents[i] == ((uint8_t *)old_data)[i],
                                 "old data unchanged after failure");
            }
        }
    }
}
