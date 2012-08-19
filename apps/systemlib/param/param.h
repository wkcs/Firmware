/**
 * @file param.h
 *
 * Global parameter store.
 */

#ifndef _SYSTEMLIB_PARAM_PARAM_H
#define _SYSTEMLIB_PARAM_PARAM_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Parameter types.
 */
typedef enum param_type_e
{
	/* globally-known parameter types */
	PARAM_TYPE_INT32 = 0,
	PARAM_TYPE_FLOAT,

	/* structure parameters; these are expected to be identified by name */
	PARAM_TYPE_STRUCT = 100,
	PARAM_TYPE_STRUCT_MAX = 16384 + PARAM_TYPE_STRUCT,

	PARAM_TYPE_UNKNOWN = 0xffff
} param_type_t;

/**
 * Parameter handle.
 *
 * Parameters are represented by parameter handles, which can
 * be obtained by looking up (or creating?) parameters.
 */
typedef uintptr_t	param_t;

/**
 * Handle returned when a parameter cannot be found.
 */
#define PARAM_INVALID	((uintptr_t)0xffffffff)

/**
 * Look up a parameter by name.
 *
 * @param name		The canonical name of the parameter being looked up.
 * @return		A handle to the parameter, or PARAM_INVALID if the parameter does not exist.
 */
__EXPORT param_t	param_find(const char *name);

/**
 * Obtain the name of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		The name assigned to the parameter, or NULL if the handle is invalid.
 */
__EXPORT const char	*param_name(param_t param);

/**
 * Obtain the type of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		The type assigned to the parameter.
 */
__EXPORT param_type_t	param_type(param_t param);

/**
 * Determine the size of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @return		The size of the parameter's value.
 */
__EXPORT size_t		param_size(param_t param);

/**
 * Obtain the scalar value of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param val		Where to return the value, assumed to point to suitable storage for the parameter type.
 *			For structures, a pointer to the structure is returned.
 * @return		Zero if the parameter's value could be returned as a scalar, nonzero otherwise.
 */
__EXPORT int		param_get(param_t param, void *val);

/**
 * Set the scalar value of a parameter.
 *
 * @param param		A handle returned by param_find or passed by param_foreach.
 * @param val		The value to set; assumed to point to a variable of the parameter type.
 *			For structures, the pointer is assumed to point to a copy of the structure.
 * @return		Zero if the parameter's value could be set from a scalar, nonzero otherwise.
 */
__EXPORT int		param_set(param_t param, void *val);

/**
 * Export changed parameters to a file.
 *
 * @param filename	The name of the file to export to.  If it exists, it will be overwritten.
 * @param only_unsaved	Only export changed parameters that have not yet been exported.
 * @return		Zero on success, nonzero on failure.
 */
__EXPORT int		param_export(const char *filename, bool only_unsaved);

/**
 * Import parameters from a file, discarding any unrecognised parameters.
 *
 * @param filename	The name of the file to import from.
 * @return		Zero on success, nonzero if an error occurred during import.
 *			Note that in the failure case, parameters may be inconsistent.
 */
__EXPORT int		param_import(const char *filename);

/**
 * Apply a function to each parameter.
 *
 * Note that the parameter set is not locked during the traversal.
 *
 * @param func		The function to invoke for each parameter.
 * @param arg		Argument passed to the function.
 * @param only_changed	If true, the function is only called for parameters whose values have
 *			been changed from the default.
 */
__EXPORT void		param_foreach(void (*func)(void *arg, param_t param), void *arg, bool only_changed);

/*
 * Macros creating static parameter definitions.
 *
 * Note that these structures are not known by name; they are 
 * collected into a section that is iterated by the parameter
 * code.
 */

/** define an int32 parameter */
#define PARAM_DEFINE_INT32(_name, _default)		\
	static const					\
	__attribute__((used, section("__param")))	\
	struct param_info_s __param__##_name = {	\
		.name = #_name,				\
		.type = PARAM_TYPE_INT32,		\
		.val.i = _default			\
	}

/** define a float parameter */
#define PARAM_DEFINE_FLOAT(_name, _default)		\
	static const					\
	__attribute__((used, section("__param")))	\
	struct param_info_s __param__##_name = {	\
		.name = #_name,				\
		.type = PARAM_TYPE_FLOAT,		\
		.val.f = _default			\
	}

/** define a parameter that points to a structure */
#define PARAM_DEFINE_STRUCT(_name, _default)		\
	static const					\
	__attribute__((used, section("__param")))	\
	struct param_info_s __param__##_name = {	\
		.name = #_name,				\
		.type = PARAM_TYPE_STRUCT + sizeof(_default), \
		.val.p = &_default;			\
	}

/**
 * Parameter value union.
 */
union param_value_u
{
	void		*p;
	int32_t		i;
	float		f;	
};

/**
 * Static parameter definition structure.
 *
 * This is normally not used by user code; see the PARAM_DEFINE macros
 * instead.
 */
struct param_info_s
{
	const char	*name;
	param_type_t	type;
	union param_value_u val;
};

#endif
