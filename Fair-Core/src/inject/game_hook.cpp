#include "game_hook.h"
#include <hook/MinHook.h>
#include <jni/jni.h>
#include "..\native\native_bridge.h"
#include "..\common\jvm\jvm_context.h"
#include "..\native\preload\preloader.h"
#include "..\common\utils.h"
typedef void(*Java_org_lwjgl_system_JNI_callP__J)(JNIEnv* env, jclass clazz, jlong lVar);
Java_org_lwjgl_system_JNI_callP__J original_nglFlush = NULL;
static LPVOID target = nullptr;

// ─── 硬编码偏移（用IDA确认后填入） ───────────────────────
#define FIELDS_OFFSET       0x240   // InstanceKlass._fields 偏移（待确认）
#define MIRROR_KLASS_OFFSET 0x68    // java.lang.Class mirror -> InstanceKlass* 偏移（待确认）
#define FIELD_SLOTS         6
// ──────────────────────────────────────────────────────────

typedef struct {
    uint64_t _mark;
    uint64_t _klass;
    int32_t  _length;
    uint16_t _data[1];
} ArrayU2;

typedef void(__fastcall* redefine_single_class_t)(
    void* self,
    void* thread,
    void* the_jclass,
    void* scratch_class
    );

static uintptr_t              g_base = 0;
static void* g_orig_redefine = nullptr;

// ─── field flags 写入 ────────────────────────────────────
void apply_field_flags(void* the_class, void* scratch_class) {
    if (!the_class || !scratch_class) {
        printf("[-] apply_field_flags: null pointer\n");
        return;
    }

    __try {
        ArrayU2* old_f = *(ArrayU2**)((uint8_t*)the_class + FIELDS_OFFSET);
        ArrayU2* new_f = *(ArrayU2**)((uint8_t*)scratch_class + FIELDS_OFFSET);

        if (!old_f || !new_f) {
            printf("[-] fields array null: old=%p new=%p\n", old_f, new_f);
            return;
        }

        int old_count = old_f->_length / FIELD_SLOTS;
        int new_count = new_f->_length / FIELD_SLOTS;
        printf("[*] field count: old=%d new=%d\n", old_count, new_count);

        if (old_count != new_count) {
            printf("[-] field count mismatch\n");
            return;
        }

        for (int i = 0; i < old_count; i++) {
            uint16_t* old_flags = &old_f->_data[i * FIELD_SLOTS];
            uint16_t* new_flags = &new_f->_data[i * FIELD_SLOTS];

            if (*old_flags == *new_flags) continue;

            printf("[*] field[%d]: 0x%04x -> 0x%04x\n", i, *old_flags, *new_flags);

            DWORD old_prot;
            VirtualProtect(old_flags, 2, PAGE_EXECUTE_READWRITE, &old_prot);
            *old_flags = *new_flags;
            VirtualProtect(old_flags, 2, old_prot, &old_prot);
        }

        printf("[+] apply_field_flags done\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("[-] exception in apply_field_flags\n");
    }
}

// ─── hook 函数 ───────────────────────────────────────────
void __fastcall hooked_redefine_single_class(
    void* self,
    void* thread,
    void* the_jclass,
    void* scratch_class)
{
    printf("[*] hooked_redefine_single_class\n");
    printf("[*] self=%p thread=%p jclass=%p scratch=%p\n",
        self, thread, the_jclass, scratch_class);

    // 先执行原函数
    ((redefine_single_class_t)g_orig_redefine)(
        self, thread, the_jclass, scratch_class);

    // jclass -> mirror oop -> InstanceKlass*
    __try {
        void* mirror = *(void**)the_jclass;
        void* the_class = *(void**)((uint8_t*)mirror + MIRROR_KLASS_OFFSET);
        printf("[*] mirror=%p the_class=%p\n", mirror, the_class);

        apply_field_flags(the_class, scratch_class);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("[-] exception resolving the_class\n");
    }
}

// ─── trampoline hook ─────────────────────────────────────
void* install_hook(void* target, void* hook) {
    uint8_t* trampoline = (uint8_t*)VirtualAlloc(
        NULL, 32, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) return nullptr;

    // 保存原始前14字节
    memcpy(trampoline, target, 14);

    // trampoline 末尾跳回 target+14
    uint64_t ret_addr = (uint64_t)((uint8_t*)target + 14);
    uint8_t jmp_back[] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    memcpy(jmp_back + 6, &ret_addr, 8);
    memcpy(trampoline + 14, jmp_back, 14);

    // target 处写入跳转到 hook
    DWORD old_prot;
    VirtualProtect(target, 14, PAGE_EXECUTE_READWRITE, &old_prot);

    uint8_t jmp_hook[] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    uint64_t hook_addr = (uint64_t)hook;
    memcpy(jmp_hook + 6, &hook_addr, 8);
    memcpy(target, jmp_hook, 14);

    VirtualProtect(target, 14, old_prot, &old_prot);
    FlushInstructionCache(GetCurrentProcess(), target, 14);

    printf("[+] hook installed: %p -> %p\n", target, hook);
    return trampoline;
}

// ─── patch compare_and_normalize ─────────────────────────
void patch_address(uintptr_t addr) {
    DWORD old_prot;
    unsigned char patch[] = { 0x31, 0xC0, 0x90, 0x90, 0x90 };
    VirtualProtect((LPVOID)addr, sizeof(patch), PAGE_EXECUTE_READWRITE, &old_prot);
    memcpy((void*)addr, patch, sizeof(patch));
    VirtualProtect((LPVOID)addr, sizeof(patch), old_prot, &old_prot);
    printf("[+] compare_and_normalize patched\n");
}

// ─── 入口 ────────────────────────────────────────────────
void patch_jvm() {
    g_base = (uintptr_t)GetModuleHandleA("jvm.dll");
    if (!g_base) {
        printf("[-] jvm.dll not found\n");
        return;
    }
    printf("[*] jvm.dll base: %p\n", (void*)g_base);

    // 1. patch 校验
    patch_address(g_base + 0x52A8C7);

    // 2. hook redefine_single_class
    void* rsc = (void*)(g_base + 0x52D0D0);
    g_orig_redefine = install_hook(rsc, (void*)hooked_redefine_single_class);
}

/// <summary>
/// 程序主入口！
/// </summary>
/// <param name="env"></param>
void Entry(JNIEnv* env) {
    //保存jvm状态 注入的时候
    JavaVM* jvm = nullptr;
    env->GetJavaVM(&jvm);
    jvm_context::set_jvm(jvm);
    //patch_jvm();
    //保存classloader
    jobject classloader = utils::jni_func::get_classloader_from_name(env, "Render thread");
    jvm_context::set_classloader(classloader);

    //加载preloader （注入Class）
   	utils::writeLog("Hook down"); 
    preloader::define_loader(env);

}


void nglFlush_Hook(JNIEnv* env, jclass clazz, jlong lVar) {
    original_nglFlush(env, clazz, lVar);
    MH_DisableHook(target);

    Entry(env);
    return;
}
bool game_hook::install_hook() {
    if (MH_Initialize() != MH_OK)
        return false;

    if (MH_CreateHookApiEx(L"lwjgl.dll",
            "Java_org_lwjgl_system_JNI_callP__J", 
            &nglFlush_Hook, 
            reinterpret_cast<LPVOID*>(&original_nglFlush), 
            &target) != MH_OK) {

        MessageBox(NULL, L"Failed to create hook for lwjgl.dll!", L"A", MB_OK | MB_ICONWARNING);
        return false;
    }
    if (MH_EnableHook(target) != MH_OK) {
        MessageBox(NULL, L"Failed to enable hook!", L"A", MB_OK);
        return false;
    }
}


