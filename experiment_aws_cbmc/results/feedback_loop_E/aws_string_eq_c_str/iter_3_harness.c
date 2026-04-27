#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_eq_c_str_harness() {
    /* parameters */
    struct aws_string *str;
    const char *c_str;

    /* assumptions */
    str = malloc(sizeof(*str) + MAX_STRING_LEN * sizeof(uint8_t));
    __CPROVER_assume(str != NULL);
    str->allocator = nondet_bool() ? aws_default_allocator() : NULL;
    str->len = nondet_size_t();
    __CPROVER_assume(str->len <= MAX_STRING_LEN);
    c_str = ensure_c_str_is_allocated(MAX_STRING_LEN);

    /* fill str->bytes with non-deterministic values */
    for (size_t i = 0; i < str->len; i++) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[str->len] = 0;  // Ensure null-termination

    /* operation under verification */
    bool result = aws_string_eq_c_str(str, c_str);

    /* assertions */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        assert(result == (memcmp(str->bytes, c_str, str->len) == 0 && c_str[str->len] == 0));
    }

    /* unchanged fields */
    assert(str->allocator == (str ? str->allocator : NULL));
    assert(str->len == (str ? str->len : 0));
    for (size_t i = 0; i <= str->len; i++) {
        assert(str->bytes[i] == (str ? str->bytes[i] : 0));
    }

    /* validity invariants */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    assert(aws_c_string_is_valid(c_str));

    /* Test with both strings being NULL */
    str = NULL;
    c_str = NULL;
    result = aws_string_eq_c_str(str, c_str);
    assert(result == true);

    /* Test with str being NULL */
    str = NULL;
    c_str = "non_null";
    result = aws_string_eq_c_str(str, c_str);
    assert(result == false);

    /* Test with c_str being NULL */
    str = (struct aws_string *)malloc(sizeof(*str) + 5 * sizeof(uint8_t));
    __CPROVER_assume(str != NULL);
    str->allocator = aws_default_allocator();
    str->len = 5;
    memcpy(str->bytes, "hello", 5);
    str->bytes[5] = 0;
    c_str = NULL;
    result = aws_string_eq_c_str(str, c_str);
    assert(result == false);

    /* Test with both strings being equal */
    str = (struct aws_string *)malloc(sizeof(*str) + 5 * sizeof(uint8_t));
    __CPROVER_assume(str != NULL);
    str->allocator = aws_default_allocator();
    str->len = 5;
    memcpy(str->bytes, "hello", 5);
    str->bytes[5] = 0;
    c_str = "hello";
    result = aws_string_eq_c_str(str, c_str);
    assert(result == true);

    /* Test with strings being different */
    str = (struct aws_string *)malloc(sizeof(*str) + 5 * sizeof(uint8_t));
    __CPROVER_assume(str != NULL);
    str->allocator = aws_default_allocator();
    str->len = 5;
    memcpy(str->bytes, "hello", 5);
    str->bytes[5] = 0;
    c_str = "world";
    result = aws_string_eq_c_str(str, c_str);
    assert(result == false);
}
