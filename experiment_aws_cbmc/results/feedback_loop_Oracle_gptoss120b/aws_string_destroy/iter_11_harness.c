#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;

    /* nondeterministically decide whether to allocate a string or use NULL */
    _Bool allocate = __CPROVER_nondet_bool();
    if (allocate) {
        size_t len = (size_t)__CPROVER_nondet_unsigned_long();
        __CPROVER_assume(len < 1024);               /* keep allocation bounded */

        uint8_t data[1024];
        for (size_t i = 0; i < len; ++i) {
            data[i] = __CPROVER_nondet_uchar();
        }
        data[len] = 0; /* NUL terminator */

        struct aws_allocator *alloc = aws_default_allocator();
        str = aws_string_new_from_array(alloc, data, len);
        __CPROVER_assume(str != NULL);
    }

    /* structural validity assumption required by the proof */
    __CPROVER_assume(!(str != NULL) || aws_string_is_valid(str));

    /* Snapshot the observable state when the string is *not* owned by an allocator */
    uint8_t *bytes_snapshot = NULL;
    size_t   len_snapshot   = 0;
    if (str != NULL && str->allocator == NULL) {
        len_snapshot = str->len;
        __CPROVER_assume(len_snapshot < 1024);
        uint8_t buffer[1024];
        bytes_snapshot = buffer;
        memcpy(bytes_snapshot, str->bytes, len_snapshot + 1);
    }

    /* Call the function under verification */
    aws_string_destroy(str);

    /* Frame condition: when the string is not owned (allocator == NULL) the memory must remain unchanged */
    if (str != NULL && str->allocator == NULL) {
        assert(str->allocator == NULL);
        assert(str->len == len_snapshot);
        for (size_t i = 0; i < len_snapshot + 1; ++i) {
            assert(((uint8_t *)str->bytes)[i] == bytes_snapshot[i]);
        }
    }
}
