/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_array_list_init_static
 */
void aws_array_list_init_static_harness() {
    /* data structure */
    struct aws_array_list list; /* Precondition: list is non-null */

    /* parameters */
    size_t item_size;
    size_t item_count;
    size_t len;

    /* assumptions */
    __CPROVER_assume(item_count > 0 && item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &len));

    /* allocate raw array */
    uint8_t *raw_array = malloc(len);
    __CPROVER_assume(raw_array != NULL);

    /* save a byte from the raw array to check immutability */
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(raw_array, len, &old_byte);

    /* perform operation under verification */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* assertions */
    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == NULL);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.current_size == item_count * item_size);
    assert(list.data == raw_array);
    assert_bytes_match((uint8_t *)list.data, raw_array, len);

    /* raw_array contents should be unchanged */
    assert_byte_from_buffer_matches(raw_array, &old_byte);
}
