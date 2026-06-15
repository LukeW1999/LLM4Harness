#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_array_eq_c_str_harness() {
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    struct store_byte_from_buffer array_storage;

    if (array_len > 0) {
        array = (uint8_t *)malloc(array_len);
        __CPROVER_assume(array != NULL);
        for (size_t i = 0; i < array_len; i++) {
            array[i] = nondet_uint8_t();
        }
        save_byte_from_array(array, array_len, &array_storage);
    }

    size_t c_str_size = array_len + 1;
    __CPROVER_assume(c_str_size <= MAX_BUFFER_SIZE && c_str_size > 0);
    uint8_t *c_str_arr = (uint8_t *)malloc(c_str_size);
    __CPROVER_assume(c_str_arr != NULL);
    for (size_t i = 0; i < array_len; i++) {
        c_str_arr[i] = nondet_uint8_t();
    }
    c_str_arr[array_len] = '\0';

    const void *array_arg = (array_len == 0) ? NULL : (const void *)array;
    const char *c_str_arg = (const char *)c_str_arr;

    struct store_byte_from_buffer c_str_storage;
    save_byte_from_array(c_str_arr, c_str_size, &c_str_storage);

    __CPROVER_assume(array_len == 0 || array != NULL);

    bool result = aws_array_eq_c_str(array_arg, array_len, c_str_arg);

    if (array_len > 0) {
        assert_byte_from_buffer_matches(array, &array_storage);
    }
    assert_byte_from_buffer_matches(c_str_arr, &c_str_storage);

    if (array_len == 0) {
        assert(result == (c_str_arr[0] == '\0'));
    } else {
        bool all_match = true;
        for (size_t i = 0; i < array_len; i++) {
            if (array[i] == '\0' || array[i] != c_str_arr[i]) {
                all_match = false;
                break;
            }
        }
        if (all_match) {
            all_match = (c_str_arr[array_len] == '\0');
        }
        assert(result == all_match);
    }
}
