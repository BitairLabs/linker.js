{
  "targets": [
    {
      "cflags!": ["-ldl", "-rdynamic"],
      "target_name": "linker_c",
      "sources": ["src/linker.c"],
      "libraries": ["<(module_root_dir)/deps/build/libffi/lib/libffi.a"]
    }
  ]
}
