#include "linker.h"
#include <assert.h>
#include <node_api.h>
#include <stdlib.h>

static napi_value Invoke(napi_env env, napi_callback_info info) {
  size_t argc = 5;
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

  char* lib_path = read_string_value(env, args[0]);
  char* fn_name = read_string_value(env, args[1]);
  napi_value fn_args = args[2];
  napi_value fn_argtypes = args[3];

  int fn_return_type;
  napi_get_value_int32(env, args[4], &fn_return_type);

  napi_value result =
      invoke(env, lib_path, fn_name, fn_argtypes, fn_args, fn_return_type);

  free(lib_path);
  free(fn_name);

  return result;
}

#define DECLARE_NAPI_METHOD(name, func)                                        \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor addDescriptor =
      DECLARE_NAPI_METHOD("invoke", Invoke);
  status = napi_define_properties(env, exports, 1, &addDescriptor);
  assert(status == napi_ok);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
