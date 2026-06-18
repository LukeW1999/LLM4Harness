#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness() {
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);
        void *mem = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(mem != NULL);
        str = (struct aws_string *)mem;
        
        struct aws_string_mut {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[1];
        };
        struct aws_string_mut *mut_str = (struct aws_string_mut *)str;
        mut_str->allocator = aws_default_allocator();
        mut_str->len = len;
        
        __CPROVER_assume(aws_string_is_valid(str));
    }

    char *c_str = NULL;
    if (nondet_bool()) {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len < MAX_BUFFER_SIZE);
        c_str = malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);
        c_str[c_len] = '\0';
    }

    struct store_byte_from_buffer old_str_byte;
    bool str_byte_saved = false;
    if (str != NULL && str->len > 0) {
        save_byte_from_array(str->bytes, str->len, &old_str_byte);
        str_byte_saved = true;
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
        if (str_byte_saved) {
            assert_byte_from_buffer_matches(str->bytes, &old_str_byte);
        }
    }
}
