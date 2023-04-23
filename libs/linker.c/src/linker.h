#include <dlfcn.h>
#include <node_api.h>
#include <stdlib.h>
#include <string.h>
#include "ffi.h"
#include <stdio.h>

typedef enum
{
  ARRAY_BUFFER,
  BOOLEAN,
  NUMBER,
  STRING
} TYPE;

typedef struct
{
  void *data;
  size_t len;
} Buffer;

char *read_string_value(napi_env env, napi_value js_value)
{
  size_t len;
  napi_get_value_string_utf8(env, js_value, NULL, 0, &len);
  char *result = (char *)malloc(len + 1);
  napi_get_value_string_utf8(env, js_value, result, len + 1, &len);
  return result;
}

Buffer read_arraybuffer_value(napi_env env, napi_value js_value)
{
  size_t len;
  void *data;
  napi_get_arraybuffer_info(env, js_value, &data, &len);
  Buffer result = {data, len};
  return result;
}

napi_value invoke(napi_env env,
                  char *lib_path,
                  char *fn_name,
                  napi_value js_argtypes,
                  napi_value js_args,
                  TYPE return_type)
{
  char *error;

  void *handle = dlopen(lib_path, RTLD_LAZY);
  if ((error = dlerror()) != NULL)
  {
    napi_throw_error(env, NULL, error);
    return NULL;
  }

  void (*fn)() = dlsym(handle, fn_name);
  if ((error = dlerror()) != NULL)
  {
    napi_throw_error(env, NULL, error);
    return NULL;
  }

  unsigned int args_len;
  napi_get_array_length(env, js_args, &args_len);

  ffi_type ffi_argtypes[args_len];
  ffi_type *ffi_argtypes_ptrs[args_len];
  void *ffi_args[args_len];
  void *ffi_args_ptrs[args_len];

  napi_value js_type;
  napi_value js_value;
  int argtypes[args_len];

  for (uint32_t i = 0; i < args_len; i++)
  {
    napi_get_element(env, js_argtypes, i, &js_type);
    napi_get_element(env, js_args, i, &js_value);
    napi_get_value_int32(env, js_type, &(argtypes[i]));

    switch (argtypes[i])
    {
    case ARRAY_BUFFER:
    {
      ffi_type ffi_arg_type;
      ffi_arg_type.size = 0;
      ffi_arg_type.alignment = 0;
      ffi_arg_type.type = FFI_TYPE_STRUCT;
      ffi_type *ffi_arg_type_elements[] = {
          &ffi_type_pointer,
          &ffi_type_ulong,
          NULL,
      };
      ffi_arg_type.elements = ffi_arg_type_elements;
      ffi_argtypes[i] = ffi_arg_type;
      ffi_argtypes_ptrs[i] = &(ffi_argtypes[i]);
      Buffer arg = read_arraybuffer_value(env, js_value);
      void *ptr = malloc(sizeof(Buffer));
      *((Buffer *)ptr) = arg;
      ffi_args[i] = ptr;
      ffi_args_ptrs[i] = ffi_args[i];
      break;
    }

    case BOOLEAN:
    {
      ffi_argtypes_ptrs[i] = &ffi_type_uint;
      bool arg;
      napi_get_value_bool(env, js_value, &arg);
      void *ptr = malloc(sizeof(bool));
      *((bool *)ptr) = arg;
      ffi_args[i] = ptr;
      ffi_args_ptrs[i] = ffi_args[i];
      break;
    }

    case NUMBER:
    {
      ffi_argtypes_ptrs[i] = &ffi_type_double;
      double arg;
      napi_get_value_double(env, js_value, &arg);
      void *ptr = malloc(sizeof(double));
      *((double *)ptr) = arg;
      ffi_args[i] = ptr;
      ffi_args_ptrs[i] = ffi_args[i];
      break;
    }

    case STRING:
    {
      ffi_argtypes_ptrs[i] = &ffi_type_pointer;
      ffi_args[i] = read_string_value(env, js_value);
      ffi_args_ptrs[i] = &(ffi_args[i]);
      break;
    }

    default:
      napi_throw_type_error(env, NULL, "Unsupported arg type.");
      return NULL;
    }
  }

  ffi_type ffi_return_type;
  ffi_type *ffi_return_type_elements[3];

  switch (return_type)
  {
  case ARRAY_BUFFER:
    ffi_return_type.size = 0;
    ffi_return_type.alignment = 0;
    ffi_return_type.type = FFI_TYPE_STRUCT;
    ffi_return_type_elements[0] = &ffi_type_pointer;
    ffi_return_type_elements[1] = &ffi_type_ulong;
    ffi_return_type_elements[2] = NULL;
    ffi_return_type.elements = ffi_return_type_elements;
    break;

  case BOOLEAN:
    ffi_return_type = ffi_type_uint;
    break;

  case NUMBER:
    ffi_return_type = ffi_type_double;
    break;

  case STRING:
    ffi_return_type = ffi_type_pointer;
    break;

  default:
    napi_throw_type_error(env, NULL, "Unsupported return type.");
    return NULL;
  }

  ffi_cif cif;
  ffi_status status = ffi_prep_cif(
      &cif, FFI_DEFAULT_ABI, args_len, &ffi_return_type, ffi_argtypes_ptrs);
  if (status != FFI_OK)
  {
    if (status == FFI_BAD_TYPEDEF)
      napi_throw_error(env, NULL, "ffi_prep_cif failed, bad typedef.");
    else if (status == FFI_BAD_ABI)
      napi_throw_error(env, NULL, "ffi_prep_cif failed, bad abi.");

    return NULL;
  }

  napi_value js_result;
  switch (return_type)
  {
  case ARRAY_BUFFER:
  {
    Buffer ffi_result;
    ffi_call(&cif, fn, &ffi_result, ffi_args_ptrs);
    napi_create_external_arraybuffer(
        env, ffi_result.data, ffi_result.len, NULL, NULL, &js_result);
    break;
  }

  case BOOLEAN:
  {
    bool ffi_result;
    ffi_call(&cif, fn, &ffi_result, ffi_args_ptrs);
    napi_create_uint32(env, ffi_result, &js_result);
    break;
  }

  case NUMBER:
  {
    double ffi_result;
    ffi_call(&cif, fn, &ffi_result, ffi_args_ptrs);
    napi_create_double(env, ffi_result, &js_result);
    break;
  }

  case STRING:
  {
    char *ffi_result;
    ffi_call(&cif, fn, &ffi_result, ffi_args_ptrs);
    napi_create_string_utf8(env, ffi_result, strlen(ffi_result), &js_result);
    break;
  }

  default:
    napi_throw_type_error(env, NULL, "Unsupported return type.");
    return NULL;
  }

  dlclose(handle);

  for (size_t i = 0; i < args_len; i++)
  {
    free(ffi_args[i]);
  }

  return js_result;
}