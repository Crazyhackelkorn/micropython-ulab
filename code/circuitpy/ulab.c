
/*
 * This file is part of the micropython-ulab project,
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "py/runtime.h"
#include "py/binary.h"
#include "py/obj.h"
#include "py/objarray.h"

#include "ulabcpy.h"
#include "../ulab.h"
#include "../ulab_create.h"
#include "../ndarray.h"
#include "../ndarray_properties.h"

#include "approx/approx.h"
#include "vector/vector.h"
#include "numerical/numerical.h"


#define ULAB_VERSION 2.1.0
#define xstr(s) str(s)
#define str(s) #s
#define ULAB_VERSION_STRING xstr(ULAB_VERSION) xstr(-) xstr(ULAB_MAX_DIMS) xstr(D) xstr(-cpy)

STATIC MP_DEFINE_STR_OBJ(ulab_version_obj, ULAB_VERSION_STRING);

// math constants
#if ULAB_NUMPY_HAS_E
mp_obj_float_t ulab_const_float_e_obj;
#endif

#if ULAB_NUMPY_HAS_INF
mp_obj_float_t numpy_const_float_inf_obj = {{&mp_type_float}, INFINITY};
#endif

#if ULAB_NUMPY_HAS_NAN
mp_obj_float_t numpy_const_float_nan_obj = {{&mp_type_float}, NAN};
#endif

#if ULAB_NUMPY_HAS_PI
mp_obj_float_t ulab_const_float_pi_obj = {{&mp_type_float}, MP_PI};
#endif


STATIC const mp_rom_map_elem_t ulab_ndarray_locals_dict_table[] = {
    // these are the methods and properties of an ndarray
    #if ULAB_MAX_DIMS > 1
        #if NDARRAY_HAS_RESHAPE
            { MP_ROM_QSTR(MP_QSTR_reshape), MP_ROM_PTR(&ndarray_reshape_obj) },
        #endif
        #if NDARRAY_HAS_TRANSPOSE
            { MP_ROM_QSTR(MP_QSTR_transpose), MP_ROM_PTR(&ndarray_transpose_obj) },
        #endif
    #endif
    #if NDARRAY_HAS_COPY
        { MP_ROM_QSTR(MP_QSTR_copy), MP_ROM_PTR(&ndarray_copy_obj) },
    #endif
    #if NDARRAY_HAS_DTYPE
        { MP_ROM_QSTR(MP_QSTR_dtype), MP_ROM_PTR(&ndarray_dtype_obj) },
    #endif
    #if NDARRAY_HAS_FLATTEN
        { MP_ROM_QSTR(MP_QSTR_flatten), MP_ROM_PTR(&ndarray_flatten_obj) },
    #endif
    #if NDARRAY_HAS_ITEMSIZE
        { MP_ROM_QSTR(MP_QSTR_itemsize), MP_ROM_PTR(&ndarray_itemsize_obj) },
    #endif
    #if NDARRAY_HAS_SHAPE
        { MP_ROM_QSTR(MP_QSTR_shape), MP_ROM_PTR(&ndarray_shape_obj) },
    #endif
    #if NDARRAY_HAS_SIZE
        { MP_ROM_QSTR(MP_QSTR_size), MP_ROM_PTR(&ndarray_size_obj) },
    #endif
    #if NDARRAY_HAS_STRIDES
        { MP_ROM_QSTR(MP_QSTR_strides), MP_ROM_PTR(&ndarray_strides_obj) },
    #endif
    #if NDARRAY_HAS_TOBYTES
        { MP_ROM_QSTR(MP_QSTR_tobytes), MP_ROM_PTR(&ndarray_tobytes_obj) },
    #endif
    #if NDARRAY_HAS_SORT
        { MP_ROM_QSTR(MP_QSTR_sort), MP_ROM_PTR(&numerical_sort_inplace_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(ulab_ndarray_locals_dict, ulab_ndarray_locals_dict_table);

const mp_obj_type_t ulab_ndarray_type = {
    { &mp_type_type },
#if defined(MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE) && defined(MP_TYPE_FLAG_EQ_HAS_NEQ_TEST)
    .flags = MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE | MP_TYPE_FLAG_EQ_HAS_NEQ_TEST,
#endif
    .name = MP_QSTR_ndarray,
    .print = ndarray_print,
    .make_new = ndarray_make_new,
    #if NDARRAY_IS_SLICEABLE
    .subscr = ndarray_subscr,
    #endif
    #if NDARRAY_IS_ITERABLE
    .getiter = ndarray_getiter,
    #endif
    #if NDARRAY_HAS_UNARY_OPS
    .unary_op = ndarray_unary_op,
    #endif
    #if NDARRAY_HAS_BINARY_OPS
    .binary_op = ndarray_binary_op,
    #endif
    .locals_dict = (mp_obj_dict_t*)&ulab_ndarray_locals_dict,
};

#if ULAB_HAS_DTYPE_OBJECT
const mp_obj_type_t ulab_dtype_type = {
    { &mp_type_type },
    .name = MP_QSTR_dtype,
    .print = ndarray_dtype_print,
    .make_new = ndarray_dtype_make_new,
};
#endif

STATIC const mp_map_elem_t ulab_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_ulab) },
    { MP_ROM_QSTR(MP_QSTR___version__), MP_ROM_PTR(&ulab_version_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ndarray), (mp_obj_t)&ulab_ndarray_type },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_array), MP_ROM_PTR(&ndarray_array_constructor_obj) },
    #if ULAB_NUMPY_HAS_FROMBUFFER
        //{ MP_ROM_QSTR(MP_QSTR_frombuffer), MP_ROM_PTR(&create_frombuffer_obj) },
    #endif
	// class constants, always included
    { MP_ROM_QSTR(MP_QSTR_bool), MP_ROM_INT(NDARRAY_BOOL) },
    { MP_ROM_QSTR(MP_QSTR_uint8), MP_ROM_INT(NDARRAY_UINT8) },
    { MP_ROM_QSTR(MP_QSTR_int8), MP_ROM_INT(NDARRAY_INT8) },
    { MP_ROM_QSTR(MP_QSTR_uint16), MP_ROM_INT(NDARRAY_UINT16) },
    { MP_ROM_QSTR(MP_QSTR_int16), MP_ROM_INT(NDARRAY_INT16) },
    { MP_ROM_QSTR(MP_QSTR_float), MP_ROM_INT(NDARRAY_FLOAT) },

    // math constants
    #if ULAB_NUMPY_HAS_E
        { MP_ROM_QSTR(MP_QSTR_e), MP_ROM_PTR(&ulab_const_float_e_obj) },
    #endif
    #if ULAB_NUMPY_HAS_INF
        { MP_ROM_QSTR(MP_QSTR_inf), MP_ROM_PTR(&numpy_const_float_inf_obj) },
    #endif
    #if ULAB_NUMPY_HAS_NAN
        { MP_ROM_QSTR(MP_QSTR_nan), MP_ROM_PTR(&numpy_const_float_nan_obj) },
    #endif
    #if ULAB_NUMPY_HAS_PI
        { MP_ROM_QSTR(MP_QSTR_pi), MP_ROM_PTR(&ulab_const_float_pi_obj) },
    #endif
    #if ULAB_HAS_DTYPE_OBJECT
        { MP_OBJ_NEW_QSTR(MP_QSTR_dtype), (mp_obj_t)&ulab_dtype_type },
    #else
        #if NDARRAY_HAS_DTYPE
        { MP_OBJ_NEW_QSTR(MP_QSTR_dtype), (mp_obj_t)&ndarray_dtype_obj },
        #endif /* NDARRAY_HAS_DTYPE */
    #endif /* ULAB_HAS_DTYPE_OBJECT */
	{ MP_OBJ_NEW_QSTR(MP_QSTR_ndarray), (mp_obj_t)&ulab_ndarray_type },
	// TODO: odd, but he following doesn't work with MP_ROM_PTR(&ndarray_array_constructor_obj)
	{ MP_OBJ_NEW_QSTR(MP_QSTR_array), (mp_obj_t)&ndarray_array_constructor_obj },
	#if ULAB_APPROX_MODULE
		{ MP_ROM_QSTR(MP_QSTR_approx), MP_ROM_PTR(&ulab_approx_module) },
	#endif
	#if ULAB_COMPARE_MODULE
		{ MP_ROM_QSTR(MP_QSTR_compare), MP_ROM_PTR(&ulab_compare_module) },
	#endif
	#if ULAB_FILTER_MODULE
		{ MP_ROM_QSTR(MP_QSTR_filter), MP_ROM_PTR(&ulab_filter_module) },
	#endif
	#if ULAB_LINALG_MODULE
		{ MP_ROM_QSTR(MP_QSTR_linalg), MP_ROM_PTR(&ulab_linalg_module) },
	#endif
	#if ULAB_NUMERICAL_MODULE
		{ MP_ROM_QSTR(MP_QSTR_numerical), MP_ROM_PTR(&ulab_numerical_module) },
	#endif
	#if ULAB_POLY_MODULE
		{ MP_ROM_QSTR(MP_QSTR_poly), MP_ROM_PTR(&ulab_poly_module) },
	#endif
	#if ULAB_VECTORISE_MODULE
		{ MP_ROM_QSTR(MP_QSTR_vector), MP_ROM_PTR(&ulab_vectorise_module) },
	#endif
};

STATIC MP_DEFINE_CONST_DICT (
    mp_module_ulab_globals,
    ulab_globals_table
);

const mp_obj_module_t ulab_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_ulab_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ulab, ulab_user_cmodule, MODULE_ULAB_ENABLED);
