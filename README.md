# fbhookfork
从 fb 的 profilo 项目里提取出来的plt hook 库，自己用

支持`armeabi-v7a`,`arm64-v8a`,`x86`

# Use

```cpp
#include "linker.h"

ssize_t write_hook(int fd, const void *buf, size_t count) {
    return CALL_PREV(write_hook, fd, buf, count);
}

hook_plt_method("libc.so", "write", (hook_func) &write_hook);


```

# Thanks
[Profilo](https://github.com/facebookincubator/profilo)
Facebook 的性能分析工具，里面黑科技很多，可以学到很多东西

