#include <dlfcn.h>
#include "linker.h"
#include <assert.h>
#include <node_api.h>
#include <stdlib.h>
#include "Python.h"

void* libpython_handle = NULL;

void finalize_fn_call(napi_env env, void* data, void* hint) {   
  PyBuffer_Release(data); 
}

void finalize_linker(napi_env env, void* data, void* hint) {
  Py_Finalize();
  dlclose(libpython_handle);
}

static napi_value Invoke(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 6;
  napi_value args[argc];

  status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  assert(status == napi_ok);

  napi_value result = invoke(env, args[0], args[1], args[2], args[3], args[4], args[5], call_fn, finalize_fn_call);

  return result;
}

#define DECLARE_NAPI_METHOD(name, func)                                                                                \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  libpython_handle = dlopen(LIBPYTHON_PATH, RTLD_LAZY | RTLD_GLOBAL);
  Py_Initialize();

  napi_status status;
  napi_property_descriptor addDescriptor = DECLARE_NAPI_METHOD("invoke", Invoke);

  status = napi_define_properties(env, exports, 1, &addDescriptor);
  assert(status == napi_ok);

  status = napi_add_finalizer(env, exports, NULL, finalize_linker, NULL, NULL);
  assert(status == napi_ok);

  return exports;
}

NAPI_MODULE(PY_LINKER, Init)
