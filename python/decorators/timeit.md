# Implementing `timeit` utility using decorators

Decorators are a very powerful tools in Python. In this module, I will implement `timeit` utility using decorators.

```python
import time
import random
import functools


def timeit(func):
    """Implementing `timeit` utility using decorators

    :param func: any function or callable entity
    """

    @functools.wraps(func)  # to avoid masking
    def counter(*args, **kwargs):
        init_time = time.perf_counter_ns()
        result = func(*args, **kwargs)
        elapsed_time = time.perf_counter_ns() - init_time
        func_name = func.__name__
        func_args = ", ".join(repr(arg) for arg in args)
        if len(kwargs):
            func_args += ", "
        func_args += ", ".join(f"{k}={repr(v)}" for k, v in kwargs.items())
        print(f"[{elapsed_time} ns] {func_name}({func_args}) -> {result!r}")
        return result
    return counter


@timeit
def exp(base, power):
    result = 1
    for _ in range(power):
        result *= base
    return result


if __name__ == "__main__":
    x = exp(98, 8)

```
