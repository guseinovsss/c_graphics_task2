#include "edu_search.h"

#include <stddef.h>

size_t edu_linear_search(const void *ptr, size_t count, size_t size, const void *key, edu_cmp cmp) {
    const unsigned char *base = (const unsigned char *)ptr;
    for (size_t i = 0; i < count; ++i) {
        const void *element = base + i * size;
        if (cmp(element, key) == 0) {
            return i;
        }
    }
    /* По контракту тестов: если не найден — вернуть count */
    return count;
}

size_t edu_binary_search(const void *ptr, size_t count, size_t size, const void *key, edu_cmp cmp) {
    const unsigned char *base = (const unsigned char *)ptr;
    size_t left = 0;
    size_t right = count; /* полуинтервал [left, right) */

    while (left < right) {
        size_t mid = left + (right - left) / 2;
        const void *element = base + mid * size;
        int res = cmp(element, key);
        if (res == 0) {
            return mid;
        } else if (res < 0) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    /* Если не найден — вернуть count (как ожидают тесты) */
    return count;
}
