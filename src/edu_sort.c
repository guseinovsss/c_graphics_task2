#include "edu_sort.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /* ptrdiff_t */

#define UNIMPLEMENTED \
    do { \
        fprintf(stderr, "[ERROR] function %s is unimplemented\n", __func__); \
        abort(); \
    } while (0)

#define UNUSED(x) (void)(x)

static void edu_swap(void *a, void *b, size_t size) {
    if (a == b) return;
    unsigned char tmp_buf[256]; /* fallback for small sizes */
    if (size <= sizeof(tmp_buf)) {
        unsigned char *tmp = tmp_buf;
        memcpy(tmp, a, size);
        memcpy(a, b, size);
        memcpy(b, tmp, size);
    } else {
        unsigned char *tmp = malloc(size);
        if (!tmp) {
            fprintf(stderr, "[ERROR] malloc failed in edu_swap\n");
            abort();
        }
        memcpy(tmp, a, size);
        memcpy(a, b, size);
        memcpy(b, tmp, size);
        free(tmp);
    }
}

// ---------- Bubble Sort ----------
void edu_bubble_sort(void *ptr, size_t count, size_t size, edu_cmp cmp) {
    if (count <= 1) return;
    unsigned char *base = (unsigned char *)ptr;
    int swapped = 1;
    size_t n = count;
    while (swapped) {
        swapped = 0;
        for (size_t i = 0; i + 1 < n; ++i) {
            void *val1 = base + i * size;
            void *val2 = base + (i + 1) * size;
            if (cmp(val1, val2) > 0) {
                edu_swap(val1, val2, size);
                swapped = 1;
            }
        }
        --n;
    }
}

// ---------- Selection Sort ----------
void edu_selection_sort(void *ptr, size_t count, size_t size, edu_cmp cmp) {
    if (count <= 1) return;
    unsigned char *base = (unsigned char *)ptr;
    for (size_t i = 0; i + 1 < count; ++i) {
        size_t min_idx = i;
        for (size_t j = i + 1; j < count; ++j) {
            if (cmp(base + j * size, base + min_idx * size) < 0) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            edu_swap(base + i * size, base + min_idx * size, size);
        }
    }
}

// ---------- Insertion Sort ----------
void edu_insertion_sort(void *ptr, size_t count, size_t size, edu_cmp cmp) {
    if (count <= 1) return;
    unsigned char *base = (unsigned char *)ptr;
    for (size_t i = 1; i < count; ++i) {
        /* временная копия ключа */
        unsigned char *key = malloc(size);
        if (!key) { fprintf(stderr, "[ERROR] malloc failed in insertion\n"); abort(); }
        memcpy(key, base + i * size, size);

        size_t j = i;
        while (j > 0 && cmp(base + (j - 1) * size, key) > 0) {
            memcpy(base + j * size, base + (j - 1) * size, size);
            --j;
        }
        memcpy(base + j * size, key, size);
        free(key);
    }
}

// ---------- Shell Sort (gap = n/2, n/=2) ----------
void edu_shell_sort(void *ptr, size_t count, size_t size, edu_cmp cmp) {
    if (count <= 1) return;
    unsigned char *base = (unsigned char *)ptr;
    for (size_t gap = count / 2; gap > 0; gap /= 2) {
        for (size_t i = gap; i < count; ++i) {
            unsigned char *temp = malloc(size);
            if (!temp) { fprintf(stderr, "[ERROR] malloc failed in shell\n"); abort(); }
            memcpy(temp, base + i * size, size);
            size_t j = i;
            while (j >= gap && cmp(base + (j - gap) * size, temp) > 0) {
                memcpy(base + j * size, base + (j - gap) * size, size);
                j -= gap;
            }
            memcpy(base + j * size, temp, size);
            free(temp);
        }
    }
}

// ---------- Quick Sort ----------
static void quick_sort_rec(unsigned char *base, ptrdiff_t left, ptrdiff_t right, size_t size, edu_cmp cmp) {
    if (left >= right) return;

    ptrdiff_t i = left;
    ptrdiff_t j = right;
    ptrdiff_t mid = left + (right - left) / 2;

    /* Копируем pivot в временный буфер (чтобы не ссылаться внутрь массива) */
    unsigned char *pivot = malloc(size);
    if (!pivot) { fprintf(stderr, "[ERROR] malloc failed in quick_sort_rec\n"); abort(); }
    memcpy(pivot, base + mid * size, size);

    while (i <= j) {
        while (cmp(base + i * size, pivot) < 0) ++i;
        while (cmp(base + j * size, pivot) > 0) --j;
        if (i <= j) {
            edu_swap(base + i * size, base + j * size, size);
            ++i;
            --j;
        }
    }
    free(pivot);

    if (left < j) quick_sort_rec(base, left, j, size, cmp);
    if (i < right) quick_sort_rec(base, i, right, size, cmp);
}

void edu_quick_sort(void *ptr, size_t count, size_t size, edu_cmp cmp) {
    if (!ptr || count <= 1) return;
    unsigned char *base = (unsigned char *)ptr;
    quick_sort_rec(base, 0, (ptrdiff_t)count - 1, size, cmp);
}

// ---------- Heap Sort ----------
static void heapify(unsigned char *base, ptrdiff_t n, ptrdiff_t i, size_t size, edu_cmp cmp) {
    ptrdiff_t largest = i;
    ptrdiff_t l = 2 * i + 1;
    ptrdiff_t r = 2 * i + 2;

    if (l < n && cmp(base + l * size, base + largest * size) > 0)
        largest = l;
    if (r < n && cmp(base + r * size, base + largest * size) > 0)
        largest = r;

    if (largest != i) {
        edu_swap(base + i * size, base + largest * size, size);
        heapify(base, n, largest, size, cmp);
    }
}

void edu_heap_sort(void *ptr, size_t count, size_t size, edu_cmp cmp) {
    if (!ptr || count <= 1) return;
    unsigned char *base = (unsigned char *)ptr;
    ptrdiff_t n = (ptrdiff_t)count;

    for (ptrdiff_t i = n / 2 - 1; i >= 0; --i) {
        heapify(base, n, i, size, cmp);
        if (i == 0) break; /* чтобы избежать бесконечного цикла, так как ptrdiff_t может быть signed */
    }

    for (ptrdiff_t i = n - 1; i > 0; --i) {
        edu_swap(base, base + i * size, size);
        heapify(base, i, 0, size, cmp);
    }
}

// ---------- Counting Sort (для int) ----------
void edu_counting_sort(void *ptr, size_t count, size_t size, edu_counting_sort_order_e order) {
    if (count == 0) return;
    if (size != sizeof(int)) {
        fprintf(stderr, "[ERROR] counting sort supports only int\n");
        abort();
    }

    int *arr = (int *)ptr;
    int min = arr[0], max = arr[0];
    for (size_t i = 1; i < count; ++i) {
        if (arr[i] < min) min = arr[i];
        if (arr[i] > max) max = arr[i];
    }

    size_t range = (size_t)(max - min + 1);
    int *count_arr = calloc(range, sizeof(int));
    if (!count_arr) { fprintf(stderr, "[ERROR] calloc failed in counting\n"); abort(); }
    int *output = malloc(count * sizeof(int));
    if (!output) { fprintf(stderr, "[ERROR] malloc failed in counting\n"); abort(); }

    for (size_t i = 0; i < count; ++i) {
        count_arr[arr[i] - min]++;
    }

    /* построим префиксные суммы */
    for (size_t i = 1; i < range; ++i) {
        count_arr[i] += count_arr[i - 1];
    }

    if (order == EDU_COUNTING_SORT_ASC) {
        for (ptrdiff_t i = (ptrdiff_t)count - 1; i >= 0; --i) {
            output[--count_arr[arr[i] - min]] = arr[i];
        }
    } else {
        /* для убывания — заполняем так, чтобы получить обратный порядок */
        for (size_t idx = 0; idx < count; ++idx) {
            /* берем элементы слева направо и помещаем их в позиции, уменьшая счётчик — затем перевернём */
            output[--count_arr[arr[idx] - min]] = arr[idx];
        }
        /* теперь перевернём output, чтобы получить строго убывающий порядок */
        for (size_t i = 0; i < count / 2; ++i) {
            int tmp = output[i];
            output[i] = output[count - 1 - i];
            output[count - 1 - i] = tmp;
        }
    }

    memcpy(arr, output, count * sizeof(int));
    free(count_arr);
    free(output);
}
