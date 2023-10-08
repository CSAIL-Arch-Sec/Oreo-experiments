from pathlib import Path


def generate_function(x):
    s = f"""void f{x}(int i, int x) {{ if (x) helper(i + (i * 100)); else printf("f({x})\\n"); }}\n"""
    return s


def main():
    s = """
#ifndef DUMMY_H
#define DUMMY_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int helper(int i) { return i + (i / 234028); }
"""

    num = 4096

    for i in range(num):
        s += generate_function(i)

    f_list = ", ".join(list(map(lambda i: f"f{i}", range(num))))
    s += f"void (*func[{num}])(int, int) = {{{f_list}}};\n"

    s += """#endif //DUMMY_H"""

    path = Path("dummy.h")
    with path.open(mode="w") as output_file:
        output_file.write(s)


if __name__ == '__main__':
    main()

