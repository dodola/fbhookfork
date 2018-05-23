# fbhookfork
从 fb 的 profilo 项目里提取出来的plt hook 库，自己用

支持`armeabi-v7a`,`arm64-v8a`,`x86`

x86的lib需要使用ndk 17去编译

# Use

```cpp

//需要 hook 的方法
std::vector<std::pair<char const *, void *>> &getFunctionHooks() {
    static std::vector<std::pair<char const *, void *>> functionHooks = {
            {"write",       reinterpret_cast<void *>(&write_hook)},
            {"__write_chk", reinterpret_cast<void *>(__write_chk_hook)},
    };
    return functionHooks;
}

//这里传入 hook 方法的库地址和本身库的地址
std::unordered_set<std::string> &getSeenLibs() {
    static bool init = false;
    static std::unordered_set<std::string> seenLibs;

    // Add this library's name to the set that we won't hook
    if (!init) {

        seenLibs.insert("libc.so");

        Dl_info info;
        if (!dladdr((void *) &getSeenLibs, &info)) {
            ALOG("Failed to find module name");
        }
        if (info.dli_fname == nullptr) {
            // Not safe to continue as a thread may block trying to hook the current
            // library
            throw std::runtime_error("could not resolve current library");
        }

        seenLibs.insert(basename(info.dli_fname));
        init = true;
    }
    return seenLibs;
}


//调用hookLoadedLibs即可
void hookLoadedLibs() {
    auto &functionHooks = getFunctionHooks();
    auto &seenLibs = getSeenLibs();
    facebook::profilo::hooks::hookLoadedLibs(functionHooks, seenLibs);
}

```

# Thanks
[Profilo](https://github.com/facebookincubator/profilo)
Facebook 的性能分析工具，里面黑科技很多，可以学到很多东西

