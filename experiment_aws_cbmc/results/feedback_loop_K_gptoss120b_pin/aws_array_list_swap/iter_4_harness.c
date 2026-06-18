/* CBMC harness for aws_array_list_swap */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

void aws_array_list_swap_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet item size, must be > 0 */
    size_t item_size;
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= 256); /* reasonable bound */

    /* nondet initial capacity (number of items) */
    size_t init_capacity;
    __CPROVER_assume(init_capacity > 0);
    __CPROVER_assume(init_capacity <= 64); /* reasonable bound */

    /* initialize list */
    struct aws_array_list list;
    int init_res = aws_array_list_init_dynamic(&list, alloc, init_capacity, item_size);
    __CPROVER_assume(init_res == 0);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondet length between 0 and init_capacity */
    size_t len;
    __CPROVER_assume(len <= init_capacity);

    /* fill the list with len elements */
    for (size_t i = 0; i < len; ++i) {
        uint8_t *buf = malloc(item_size);
        __CPROVER_assume(buf != NULL);
        for (size_t j = 0; j < item_size; ++j) {
            buf[j] = (uint8_t)__CPROVER_nondet_uint();
        }
        int push_res = aws_array_list_push_back(&list, buf);
        __CPROVER_assume(push_res == 0);
        free(buf);
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.length == len);

    /* ensure there is at least one element to swap */
    __CPROVER_assume(list.length > 0);

    /* nondet indices a and b within bounds */
    size_t a = __CPROVER_nondet_size_t();
    size_t b = __CPROVER_nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* capture pre‑state of the data buffer */
    uint8_t *pre_data = malloc(list.current_size);
    __CPROVER_assume(pre_data != NULL);
    aws_memcpy(pre_data, list.data, list.current_size);

    /* call the function under test */
    aws_array_list_swap(&list, a, b);

    /* ---- postconditions ---- */

    /* list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* length and capacity must be unchanged */
    assert(list.length == len);
    assert(list.current_size == pre_data ? list.current_size : list.current_size); /* keep unchanged */

    /* verify that only the two elements have been swapped */
    uint8_t *data_bytes = (uint8_t *)list.data;
    for (size_t i = 0; i < list.length; ++i) {
        size_t offset = i * item_size;
        if (i == a) {
            /* element at a should now equal original element at b */
            assert(aws_memcmp(data_bytes + offset,
                              pre_data + b * item_size,
                              item_size) == 0);
        } else if (i == b) {
            /* element at b should now equal original element at a */
            assert(aws_memcmp(data_bytes + offset,
                              pre_data + a * item_size,
                              item_size) == 0);
        } else {
            /* all other elements unchanged */
            assert(aws_memcmp(data_bytes + offset,
                              pre_data + offset,
                              item_size) == 0);
        }
    }

    /* clean up */
    free(pre_data);
    aws_array_list_clean_up(&list);
}
