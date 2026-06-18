#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_push_back_harness(void) {
    /*--- Set up a dynamic array list ---------------------------------------*/
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size < 1024); /* reasonable bound */

    size_t initial_item_allocation = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= 256); /* bound */

    int init_ret = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);
    __CPROVER_assume(init_ret == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /*--- Set a nondeterministic valid length (<= capacity) ---------------*/
    size_t capacity = list.current_size / list.item_size;
    size_t length = nondet_size_t();
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /*--- Allocate a nondeterministic input value --------------------------*/
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_size; ++i) {
        val[i] = nondet_uint8_t();
    }

    /*--- Snapshot pre‑state ------------------------------------------------*/
    uint8_t *old_data = NULL;
    if (list.current_size > 0) {
        old_data = malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, list.current_size);
    }
    struct aws_array_list old_list = list;

    /*--- Call the function under test --------------------------------------*/
    int ret = aws_array_list_push_back(&list, val);

    /*--- General post‑condition: list must remain valid --------------------*/
    assert(aws_array_list_is_valid(&list));

    /*--- Return‑value correctness ------------------------------------------*/
    if (ret == AWS_OP_SUCCESS) {
        assert(list.length == old_list.length + 1);
        assert(list.current_size / list.item_size >= list.length);
    } else {
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        assert(list.data == old_list.data);
    }

    /*--- Frame conditions (memory not modified beyond contract) ------------*/
    if (list.current_size > 0 && old_data != NULL) {
        size_t old_len_bytes = old_list.length * old_list.item_size;
        size_t new_len_bytes = list.length * list.item_size;

        if (old_len_bytes > 0) {
            assert(memcmp(old_data, list.data, old_len_bytes) == 0);
        }

        if (new_len_bytes < list.current_size) {
            size_t tail_len = list.current_size - new_len_bytes;
            assert(memcmp(old_data + new_len_bytes,
                          (uint8_t *)list.data + new_len_bytes,
                          tail_len) == 0);
        }

        if (ret == AWS_OP_SUCCESS) {
            uint8_t *new_elem = (uint8_t *)list.data + old_len_bytes;
            assert(memcmp(new_elem, val, list.item_size) == 0);
        }
    }

    /*--- Clean up ----------------------------------------------------------*/
    if (list.alloc && list.data) {
        aws_mem_release(list.alloc, list.data);
    }
    free(val);
    free(old_data);
}
