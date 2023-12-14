from pathlib import Path


def generate_function(x):
    s = f"""void f{x}(int i, int x) {{ if (x) helper(i + (i * 100)); else pr_info("f({x})\\n"); }}\n"""
    return s


def generate_function_call(x):
    return f"\tf{x}({x * x}, 1);"


def generate_flush_tlb_btb(n):
    call_list = "\n".join(list(map(lambda i: generate_function_call(i), list(range(n)))))
    return f"void flush_tlb_btb(void) {{\n" \
           f"{call_list}\n" \
           f"}}\n"


def main():
    s = """
#ifndef DUMMY_H
#define DUMMY_H
#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include "utils.h"

int helper(int i) { return i + (i / 234028); }
"""

    num = 4096

    for i in range(num):
        s += generate_function(i)

    f_list = ", ".join(list(map(lambda i: f"f{i}", range(num))))
    s += f"void (*func[{num}])(int, int) = {{{f_list}}};\n"

    s += generate_flush_tlb_btb(num)

    s += """#endif //DUMMY_H"""

    path = Path("dummy.h")
    with path.open(mode="w") as output_file:
        output_file.write(s)


if __name__ == '__main__':
    main()

