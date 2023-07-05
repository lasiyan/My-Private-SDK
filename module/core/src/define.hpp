#ifndef __ROWEN_SDK_CORE_DEFINE_HPP__
#define __ROWEN_SDK_CORE_DEFINE_HPP__

#ifndef SAFE_DELETE
  #define SAFE_DELETE(ptr) \
    if (ptr) {             \
      delete ptr;          \
      ptr = nullptr;       \
    }
#endif

#ifndef SAFE_DELETE_ARRAY
  #define SAFE_DELETE_ARRAY(ptr) \
    if (ptr) {                   \
      delete[] ptr;              \
      ptr = nullptr;             \
    }
#endif

#ifndef HIWORD
  #define HIWORD(l) ((uint16_t)((((uint32_t)(l)) >> 16) & 0xffff))
#endif

#ifndef HIBYTE
  #define HIBYTE(w) ((uint8_t)(((uint16_t)(w) >> 8) & 0xff))
#endif

#ifndef LOWORD
  #define LOWORD(l) ((uint16_t)(((uint32_t)(l)) & 0xffff))
#endif

#ifndef LOBYTE
  #define LOBYTE(w) ((uint8_t)(w))
#endif

#ifndef SNPRINTF
  #undef SNPRINTF
  #define SNPRINTF(buf, fmt, ...)                   \
    {                                               \
      snprintf(buf, sizeof(buf), fmt, __VA_ARGS__); \
    }
#endif

#ifndef D_CAST
  #define D_CAST(type, ptr) (dynamic_cast<type*>(ptr))
#endif

#ifndef S_CAST
  #define S_CAST(type, ptr) (static_cast<type*>(ptr))
#endif

#ifndef R_CAST
  #define R_CAST(type, ptr) (reinterpret_cast<type*>(ptr))
#endif

#endif  //__ROWEN_SDK_CORE_DEFINE_HPP__