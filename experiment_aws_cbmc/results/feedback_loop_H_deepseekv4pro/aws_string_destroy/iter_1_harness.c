#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Harness for aws_string_destroy: verifies that a dynamically allocated string is
 * freed, a statically allocated string is left unchanged, and NULL has no effect.
 */
void aws_string_destroy_harness() {
    /* noreturn variable initializations */
    struct aws_string *str = NULL;
    bool is_null = nondet_bool();
    bool is_dynamic = false;

    /* If the string is not NULL, decide if it is dynamically or statically allocated */
    if (!is_null) {
        is_dynamic = nondet_bool();
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);   /* bound the string length */

        /* Create a temporary buffer with random contents (including a null terminator) */
        uint8_t *data = malloc(len + 1);
        __CPROVER_assume(data != NULL);
        for (size_t i = 0; i < len; i++) {
            data[i] = nondet_uint8_t();
        }
        data[len] = '\0';

        if (is_dynamic) {
            /* Dynamic string: allocate via the library using the default allocator */
            str = aws_string_new_from_array(aws_default_allocator(), data, len);
            __CPROVER_assume(str != NULL);
            /* The library already copied the data, free our temporary buffer */
            free(data);
            /* str will be freed by aws_string_destroy – no manual cleanup later */
        } else {
            /* Static (allocator == NULL) string: allocate raw memory and construct by hand */
            /* Allocate enough space for the header plus len+1 bytes (data + null) */
            size_t alloc_size = sizeof(struct aws_string) + len;
            str = malloc(alloc_size);
            __CPROVER_assume(str != NULL);

            /* Use a non‑const overlay to initialise the fields */
            struct aws_string_nonconst {
                struct aws_allocator *allocator;
                size_t len;
                uint8_t bytes[];
            } *nc = (void *)str;

            nc->allocator = NULL;
            nc->len = len;
            /* Copy the generated data plus the null terminator */
            memcpy(nc->bytes, data, len + 1);
            free(data);

            /* We will need to free this string manually after the test,
             * because aws_string_destroy must not free a static string.
             */
        }
    } else {
        str = NULL;
    }

    /* Save the contents of a static string for later validation */
    size_t saved_len = 0;
    uint8_t *saved_data = NULL;
    if (!is_null && !is_dynamic) {
        saved_len = str->len;
        saved_data = malloc(saved_len + 1);
        __CPROVER_assume(saved_data != NULL);
        memcpy(saved_data, aws_string_bytes(str), saved_len + 1);
    }

    /* ----- Call the function under test ----- */
    aws_string_destroy(str);

    /* ----- Postconditions ----- */
    if (is_null) {
        /* NULL input: nothing to check */
    } else if (!is_dynamic) {
        /* Static string (allocator == NULL): the function must not free it and
         * must not modify its contents. */
        assert(aws_string_is_valid(str));
        assert_bytes_match(aws_string_bytes(str), saved_data, saved_len + 1);

        /* The harness is responsible for deallocating the static string */
        free(saved_data);
        free(str);
    } else {
        /* Dynamic string: the function freed the memory.
         * CBMC will automatically check that no double‑free or invalid‑free occurred.
         * No post‑call accesses are allowed here. */
    }
}
