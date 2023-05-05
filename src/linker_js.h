#include <node_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARG_MAX 256

typedef enum { linker_type_buffer, linker_type_bool, linker_type_number, linker_type_string } linker_type;

typedef struct {
  void* data;
  size_t len;
} linker_buffer;

typedef struct {
  linker_type type;
  void* value;
} linker_value;

char* read_string_value(napi_env env, napi_value js_value) {
  size_t len;
  napi_get_value_string_utf8(env, js_value, NULL, 0, &len);
  char* result = (char*)malloc(len + 1);
  napi_get_value_string_utf8(env, js_value, result, len + 1, &len);
  return result;
}

linker_buffer read_arraybuffer_value(napi_env env, napi_value js_value) {
  size_t len;
  void* data;
  napi_get_arraybuffer_info(env, js_value, &data, &len);
  linker_buffer result = {data, len};
  return result;
}

napi_value invoke(napi_env env, napi_value js_lib_dir, napi_value js_lib_name, napi_value js_fn_name,
                  napi_value js_args, napi_value js_argtypes, napi_value js_return_type,
                  void (*call_fn)(char*, char*, char*, u_int8_t, linker_value[ARG_MAX], linker_type, void*),
                  void (*finalize)(napi_env, void*, void*)) {
  char* lib_dir = read_string_value(env, js_lib_dir);
  char* lib_name = read_string_value(env, js_lib_name);
  char* fn_name = read_string_value(env, js_fn_name);

  int return_type;
  napi_get_value_int32(env, js_return_type, &return_type);

  uint32_t args_len;
  napi_get_array_length(env, js_args, &args_len);

  linker_value args[args_len];

  napi_value js_type;
  napi_value js_value;
  for (uint32_t i = 0; i < args_len; i++) {
    double arg_type;
    napi_get_element(env, js_argtypes, i, &js_type);
    napi_get_element(env, js_args, i, &js_value);
    napi_get_value_double(env, js_type, &arg_type);

    void* arg_ref;
    switch ((linker_type)arg_type) {
      case linker_type_buffer: {
        linker_buffer arg = read_arraybuffer_value(env, js_value);
        arg_ref = malloc(sizeof(linker_buffer));
        *((linker_buffer*)arg_ref) = arg;
        break;
      }

      case linker_type_bool: {
        bool arg;
        napi_get_value_bool(env, js_value, &arg);
        arg_ref = malloc(sizeof(bool));
        *((bool*)arg_ref) = arg;
        break;
      }

      case linker_type_number: {
        double arg;
        napi_get_value_double(env, js_value, &arg);
        arg_ref = malloc(sizeof(double));
        *((double*)arg_ref) = arg;
        break;
      }

      case linker_type_string:
        arg_ref = read_string_value(env, js_value);
        break;
    }

    args[i] = (linker_value){.type = arg_type, .value = arg_ref};
  }

  napi_value js_result;

  switch (return_type) {
    case linker_type_buffer: {
      linker_buffer result;
      call_fn(lib_dir, lib_name, fn_name, args_len, args, return_type, &result);
      napi_create_external_arraybuffer(env, result.data, result.len, finalize, NULL, &js_result);
      break;
    }

    case linker_type_bool: {
      bool result;
      call_fn(lib_dir, lib_name, fn_name, args_len, args, return_type, &result);
      napi_create_uint32(env, result, &js_result);
      break;
    }

    case linker_type_number: {
      double result;
      call_fn(lib_dir, lib_name, fn_name, args_len, args, return_type, &result);
      napi_create_double(env, result, &js_result);
      break;
    }

    case linker_type_string: {
      char* result;
      call_fn(lib_dir, lib_name, fn_name, args_len, args, return_type, &result);
      napi_create_string_utf8(env, result, strlen(result), &js_result);
      free(result);
      break;
    }
  }

  for (size_t i = 0; i < args_len; i++) {
    free((args[i]).value);
  }

  free(lib_dir);
  free(lib_name);
  free(fn_name);

  return js_result;
}