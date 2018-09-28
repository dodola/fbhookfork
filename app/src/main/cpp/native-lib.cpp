#include <jni.h>
#include <string>

#include <atomic>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sstream>
#include <unordered_set>
#include <android/log.h>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/system_properties.h>
#include <vector>
#include <linker.h>
#include <hooks.h>

#define  LOG_TAG    "HOOOOOOOOK"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

int *atrace_marker_fd = nullptr;
std::atomic<uint64_t> *atrace_enabled_tags = nullptr;
std::atomic<uint64_t> original_tags(UINT64_MAX);
std::atomic<bool> systrace_installed;
bool first_enable = true;


void log_systrace(int fd, const void *buf, size_t count) {
    const char *msg = reinterpret_cast<const char *>(buf);
    ALOG("waitme===%s", msg);

}


ssize_t write_hook(int fd, const void *buf, size_t count) {
    log_systrace(fd, buf, count);
    return CALL_PREV(write_hook, fd, buf, count);
}

ssize_t __write_chk_hook(int fd, const void *buf, size_t count, size_t buf_size) {
    log_systrace(fd, buf, count);
    return CALL_PREV(__write_chk_hook, fd, buf, count, buf_size);
}


std::vector<std::pair<char const *, void *>> &getFunctionHooks() {
    static std::vector<std::pair<char const *, void *>> functionHooks = {
            {"write",       reinterpret_cast<void *>(&write_hook)},
            {"__write_chk", reinterpret_cast<void *>(__write_chk_hook)},
    };
    return functionHooks;
}

// Returns the set of libraries that we don't want to hook.
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

void hookLoadedLibs() {
    auto &functionHooks = getFunctionHooks();
    auto &seenLibs = getSeenLibs();

    facebook::profilo::hooks::hookLoadedLibs(functionHooks, seenLibs);
}

static int getAndroidSdk() {
    static auto android_sdk = ([] {
        char sdk_version_str[PROP_VALUE_MAX];
        __system_property_get("ro.build.version.sdk", sdk_version_str);
        return atoi(sdk_version_str);
    })();
    return android_sdk;
}

void installSystraceSnooper() {
    auto sdk = getAndroidSdk();
    {
        std::string lib_name("libcutils.so");
        std::string enabled_tags_sym("atrace_enabled_tags");
        std::string fd_sym("atrace_marker_fd");

        if (sdk < 18) {
            lib_name = "libutils.so";
            // android::Tracer::sEnabledTags
            enabled_tags_sym = "_ZN7android6Tracer12sEnabledTagsE";
            // android::Tracer::sTraceFD
            fd_sym = "_ZN7android6Tracer8sTraceFDE";
        }

        void *handle;
        if (sdk < 21) {
            handle = dlopen(lib_name.c_str(), RTLD_LOCAL);
        } else {
            handle = dlopen(nullptr, RTLD_GLOBAL);
        }

        atrace_enabled_tags =
                reinterpret_cast<std::atomic<uint64_t> *>(
                        dlsym(handle, enabled_tags_sym.c_str()));

        if (atrace_enabled_tags == nullptr) {
            throw std::runtime_error("Enabled Tags not defined");
        }

        atrace_marker_fd =
                reinterpret_cast<int *>(dlsym(handle, fd_sym.c_str()));

        if (atrace_marker_fd == nullptr) {
            throw std::runtime_error("Trace FD not defined");
        }
        if (*atrace_marker_fd == -1) {
            throw std::runtime_error("Trace FD not valid");
        }
    }

    if (linker_initialize()) {
        throw std::runtime_error("Could not initialize linker library");
    }

    hookLoadedLibs();

    systrace_installed = true;
}

void enableSystrace() {
    if (!systrace_installed) {
        return;
    }

    if (!first_enable) {
        // On every enable, except the first one, find if new libs were loaded
        // and install systrace hook for them
        try {
            hookLoadedLibs();
        } catch (...) {
            // It's ok to continue if the refresh has failed
        }
    }
    first_enable = false;

    auto prev = atrace_enabled_tags->exchange(UINT64_MAX);
    if (prev !=
        UINT64_MAX) { // if we somehow call this twice in a row, don't overwrite the real tags
        original_tags = prev;
    }
}

void restoreSystrace() {
    if (!systrace_installed) {
        return;
    }

    uint64_t tags = original_tags;
    if (tags != UINT64_MAX) { // if we somehow call this before enableSystrace, don't screw it up
        atrace_enabled_tags->store(tags);
    }
}

bool installSystraceHook() {
    try {
        ALOG("===============install systrace hoook==================");
        installSystraceSnooper();
        return true;
    } catch (const std::runtime_error &e) {
        return false;
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_dodola_tracehooker_Atrace_enableSystraceNative(JNIEnv *env, jclass type) {
    enableSystrace();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dodola_tracehooker_Atrace_restoreSystraceNative(JNIEnv *env, jclass type) {
    restoreSystrace();

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_dodola_tracehooker_Atrace_installSystraceHook(JNIEnv *env, jclass type) {
    installSystraceHook();
    return static_cast<jboolean>(true);
}
