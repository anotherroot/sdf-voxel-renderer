#ifndef ARC_LOG
#define ARC_LOG
#include <stdio.h>
namespace arc {
/* Black 30 40 */
/* Red 31 41 */
/* Green 32 42 */
/* Yellow 33 43 */
/* Blue 34 44 */
/* Magenta 35 45 */
/* Cyan 36 46 */
/* White 37 47 */
/* Bright Black 90 100 */
/* Bright Red 91 101 */
/* Bright Green 92 102 */
/* Bright Yellow 93 103 */
/* Bright Blue 94 104 */
/* Bright Magenta 95 105 */
/* Bright Cyan 96 106 */
/* Bright White 97 107 */

enum log_col {
  Black = 30,
  Red = 31,
  Green = 32,
  Yellow = 33,
  Blue = 34,
  Magenta = 35,
  Cyan = 36,
  White = 37,
  Bright_Black = 90,
  Bright_Red = 91,
  Bright_Green = 92,
  Bright_Yellow = 93,
  Bright_Blue = 94,
  Bright_Magenta = 95,
  Bright_Cyan = 96,
  Bright_White = 97,
};

} // namespace arc
namespace arc::impl {

static char buff[256];

#define Debug // TODO: with cmake
#ifdef Debug
#define LOG_INIT() arc::impl::Log::Init();
#define ARC_ASSERT(x, ...)                                                     \
  {                                                                            \
    if (!(x)) {                                                                \
      fprintf(stderr, __VA_ARGS__);                                            \
      throw std::exception();                                                  \
    }                                                                          \
  }
#define logi(...)                                                              \
  {                                                                            \
    sprintf(arc::impl::buff, __VA_ARGS__);                                     \
    printf("Info => %s:  %s\n", __func__, arc::impl::buff);                    \
  }
#define loge(...)                                                              \
  {                                                                            \
    int l = sprintf(arc::impl::buff, __VA_ARGS__);                             \
    printf("\x1B[31mError => %.*s:  %.*s\033[0m\n", (int)sizeof(__func__),          \
           __func__, l, arc::impl::buff);                                      \
  }

#define logc(c, b, ...)                                                        \
  {                                                                            \
    int l = sprintf(arc::impl::buff, __VA_ARGS__);                             \
    printf("\033[" #c ";" #b "mError => %.*s:  %.*s\033[0m\n",                 \
           sizeof(__func__), __func__, l, arc::impl::buff);                    \
  }

#else
#define LOG_INIT()
#define ARC_ASSERT(x, ...)
#define logi(...)
#define loge(...)
#endif
} // namespace arc::impl
#endif
