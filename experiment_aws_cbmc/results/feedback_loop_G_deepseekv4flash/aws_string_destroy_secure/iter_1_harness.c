#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

/* We need a bound for the string length */
#define MAX_STRING_LEN 64

/* Inline aws_string_is_valid for harness purposes */
static inline bool aws_string_is_valid(const struct aws_string *str) {
    if (str == NULL) return false;
    if (!AWS_MEM_IS_READABLE(str, sizeof(struct aws_string))) return false;
    size_t len = str->len;
    if (len > MAX_STRING_LEN) return false;
    if (len > 0) {
        if (!AWS_MEM_IS_READABLE(str->bytes, len + 1)) return false;
        /* A null terminator must be present after the data */
        if (str->bytes[len] != 0) return false;
    }
    /* If allocator is non-null, we assume it is a valid pointer (cannot fully check in harness) */
    return true;
}

void aws_string_destroy_secure_harness() {
    /* Nondeterministic: choose whether str is NULL or a valid string */
    bool is_null = nondet_bool();
    struct aws_string *str = is_null ? NULL : (struct aws_string *)malloc(sizeof(struct aws_string) + MAX_STRING_LEN + 1);

    if (str != NULL) {
        /* Set up fields, casting away const for modeling */
        struct aws_allocator *alloc = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
        *(struct aws_allocator **)&str->allocator = alloc;
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);
        *(size_t *)&str->len = len;
        /* Initialize bytes with arbitrary values and ensure null terminator */
        for (size_t i = 0; i < len; i++) {
            str->bytes[i] = nondet_uint8_t();
        }
        str->bytes[len] = 0; /* null terminator */
        /* Ensure validity */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Save state for post-conditions when allocator is NULL and str exists */
    bool allocator_nonnull = (str != NULL && str->allocator != NULL);
    size_t saved_len = 0;
    struct store_byte_from_buffer saved_bytes[MAX_STRING_LEN];
    if (str != NULL && !allocator_nonnull && str->len > 0) {
        saved_len = str->len;
        for (size_t i = 0; i < saved_len; i++) {
            save_byte_from_array(&str->bytes[i], 1, &saved_bytes[i]);
        }
    }

    /* Call the function */
    aws_string_destroy_secure(str);

    /* Postconditions */
    if (str == NULL) {
        /* Nothing happened */
        ;
    } else if (allocator_nonnull) {
        /* Memory freed: we cannot safely access str. CBMC will check no use-after-free. */
        /* No assertions on freed memory. */
    } else {
        /* str != NULL and allocator is NULL: bytes zeroed but memory not freed */
        assert(str->len == saved_len);
        for (size_t i = 0; i < saved_len; i++) {
            assert(str->bytes[i] == 0);
        }
        assert(str->allocator == NULL);
        /* Validity invariant still holds, as aws_string_is_valid only requires readable memory */
        assert(aws_string_is_valid(str));
    }
}
