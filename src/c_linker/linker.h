#include <dlfcn.h>
#include <ffi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../linker_js.h"

void call_fn(char *lib_dir, char *lib_name, char *fn_name, u_int8_t args_len, linker_value args[ARG_MAX], linker_type return_type, void *result)
{
  char *error;

  char lib_path[strlen(lib_dir) + strlen(lib_name) + 1];
  strcpy(lib_path, lib_dir);
  strcat(lib_path, lib_name);

  void *handle = dlopen(lib_path, RTLD_LAZY);
  if ((error = dlerror()) != NULL)
  {
    printf("%s", error);
    result = NULL;
    return;
  }

  void *fn = dlsym(handle, fn_name);
  if ((error = dlerror()) != NULL)
  {
    printf("%s", error);
    result = NULL;
    return;
  }

  ffi_type ffi_argtypes[args_len];
  ffi_type *ffi_argtypes_refs[args_len];
  void *ffi_args[args_len];
  void *ffi_args_refs[args_len];

  ffi_type *ffi_arg_type_elements[args_len][3];

  for (u_int8_t i = 0; i < args_len; i++)
  {
    linker_value arg = args[i];

    switch (arg.type)
    {
    case linker_type_buffer:
    {
      ffi_type ffi_arg_type;
      ffi_arg_type.size = 0;
      ffi_arg_type.alignment = 0;
      ffi_arg_type.type = FFI_TYPE_STRUCT;
      ffi_arg_type_elements[i][0] = &ffi_type_pointer;
      ffi_arg_type_elements[i][1] = &ffi_type_ulong,
      ffi_arg_type_elements[i][2] = NULL;
      ffi_arg_type.elements = ffi_arg_type_elements[i];
      ffi_argtypes[i] = ffi_arg_type;
      ffi_argtypes_refs[i] = &(ffi_argtypes[i]);
      ffi_args[i] = arg.value;
      ffi_args_refs[i] = ffi_args[i];
      break;
    }

    case linker_type_bool:
    {
      ffi_argtypes_refs[i] = &ffi_type_uint;
      ffi_args[i] = arg.value;
      ffi_args_refs[i] = ffi_args[i];
      break;
    }

    case linker_type_number:
    {
      ffi_argtypes_refs[i] = &ffi_type_double;
      ffi_args[i] = arg.value;
      ffi_args_refs[i] = ffi_args[i];
      break;
    }

    case linker_type_string:
    {
      ffi_argtypes_refs[i] = &ffi_type_pointer;
      ffi_args[i] = arg.value;
      ffi_args_refs[i] = &(ffi_args[i]);
      break;
    }
    }
  }

  ffi_type ffi_return_type;
  ffi_type *ffi_return_type_elements[3];

  switch (return_type)
  {
  case linker_type_buffer:
    ffi_return_type.size = 0;
    ffi_return_type.alignment = 0;
    ffi_return_type.type = FFI_TYPE_STRUCT;
    ffi_return_type_elements[0] = &ffi_type_pointer;
    ffi_return_type_elements[1] = &ffi_type_ulong;
    ffi_return_type_elements[2] = NULL;
    ffi_return_type.elements = ffi_return_type_elements;
    break;

  case linker_type_bool:
    ffi_return_type = ffi_type_uint;
    break;

  case linker_type_number:
    ffi_return_type = ffi_type_double;
    break;

  case linker_type_string:
    ffi_return_type = ffi_type_pointer;
    break;
  }

  ffi_cif cif;
  ffi_status status = ffi_prep_cif(
      &cif, FFI_DEFAULT_ABI, args_len, &ffi_return_type, ffi_argtypes_refs);
  if (status != FFI_OK)
  {
    if (status == FFI_BAD_TYPEDEF)
      printf("%s", "ffi_prep_cif failed, bad typedef.");
    else if (status == FFI_BAD_ABI)
      printf("%s", "ffi_prep_cif failed, bad abi.");

    result = NULL;
    return;
  }

  ffi_call(&cif, FFI_FN(fn), result, ffi_args_refs);

  dlclose(handle);
}