#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int global_var = 150;
int bss_var;

struct segment_entry {
    const char *name;
    const char *segment;
    uintptr_t address;
};

static void print_entry(const struct segment_entry *entry)
{
    printf("%-16s %p  (%s)\n", entry->name, (void *)entry->address, entry->segment);
}

int main(void)
{
    int local_var = 30;
    static int local_static = 20;
    int *heap_var = malloc(sizeof(*heap_var));

    if (heap_var == NULL) {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }
    *heap_var = 500;

    struct segment_entry entries[] = {
        {"main()", "Text", (uintptr_t)main},
        {"global_var", "Data", (uintptr_t)(void *)&global_var},
        {"bss_var", "BSS", (uintptr_t)(void *)&bss_var},
        {"local_static", "Data", (uintptr_t)(void *)&local_static},
        {"local_var", "Stack", (uintptr_t)(void *)&local_var},
        {"heap_var ptr", "Stack (pointer)", (uintptr_t)(void *)&heap_var},
        {"*heap_var", "Heap", (uintptr_t)(void *)heap_var}
    };
    size_t count = sizeof(entries) / sizeof(entries[0]);

    printf(".......Variable Addresses.......\n\n");
    for (size_t i = 0; i < count; i++) {
        print_entry(&entries[i]);
    }

    size_t high = 0;
    size_t low = 0;
    for (size_t i = 1; i < count; i++) {
        if (entries[i].address > entries[high].address) high = i;
        if (entries[i].address < entries[low].address) low = i;
    }

    printf("\nHighest address: 0x%zx  (%s: %s)\n",
           entries[high].address, entries[high].name, entries[high].segment);
    printf("Lowest address:  0x%zx  (%s: %s)\n",
           entries[low].address, entries[low].name, entries[low].segment);

    uintptr_t stack_addr = (uintptr_t)(void *)&local_var;
    uintptr_t heap_addr = (uintptr_t)(void *)heap_var;
    printf("Stack - Heap:    0x%zx bytes (%zu KB)\n",
           stack_addr - heap_addr, (stack_addr - heap_addr) / 1024);

    free(heap_var);
    return 0;
}
