#pragma once

#if ZHM_TARGET == 3
#include <Generated/HM3/ZHMProperties.h>
#elif ZHM_TARGET == 2
#include <Generated/HM2/ZHMProperties.h>
#elif ZHM_TARGET == 2016
#include <Generated/HM2016/ZHMProperties.h>
#elif ZHM_TARGET == 2012
#include <Generated/HMA/ZHMProperties.h>
#endif
