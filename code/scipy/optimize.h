
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Zoltán Vörös
 *               
*/

#ifndef _SCIPY_OPTIMIZE_
#define _SCIPY_OPTIMIZE_

#include "../ulab_tools.h"

#define     OPTIMIZE_EPS          MICROPY_FLOAT_CONST(1.0e-4)
#define     OPTIMIZE_NONZDELTA    MICROPY_FLOAT_CONST(0.05)
#define     OPTIMIZE_ZDELTA       MICROPY_FLOAT_CONST(0.00025)
#define     OPTIMIZE_ALPHA        MICROPY_FLOAT_CONST(1.0)
#define     OPTIMIZE_BETA         MICROPY_FLOAT_CONST(2.0)
#define     OPTIMIZE_GAMMA        MICROPY_FLOAT_CONST(0.5)
#define     OPTIMIZE_DELTA        MICROPY_FLOAT_CONST(0.5)

extern mp_obj_module_t ulab_scipy_optimize_module;

MP_DECLARE_CONST_FUN_OBJ_KW(optimize_bisect_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(optimize_curve_fit_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(optimize_fmin_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(optimize_newton_obj);

#endif /* _SCIPY_OPTIMIZE_ */
