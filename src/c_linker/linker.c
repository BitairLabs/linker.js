#include "linker.h"
#include <assert.h>
#include <node_api.h>
#include <stdlib.h>

void finalize_fn_call(napi_env env, void* data, void* hint) { free(data); }

static napi_value Invoke(napi_env env, napi_callback_info info) {
  size_t argc = 6;
  napi_value args[argc];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  napi_value result = invoke(env, args[0], args[1], args[2], args[3], args[4], args[5], call_fn, finalize_fn_call);
  return result;
}

#define DECLARE_NAPI_METHOD(name, func)                                                                                \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor addDescriptor = DECLARE_NAPI_METHOD("invoke", Invoke);
  status = napi_define_properties(env, exports, 1, &addDescriptor);
  assert(status == napi_ok);
  return exports;
}

NAPI_MODULE(C_LINKER, Init)
