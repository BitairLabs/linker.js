# Intro

Linker.js is a project that empowers a Node.js application to access libraries that are written in other languages. Linker.js is part of [Concurrent.js](https://github.com/bitair-org/concurrent.js).

# Features

- Language support
  - [x] C via [Linker.c](#linkerc)
  - [x] C++ via [Linker.c](#linkerc)
  - [x] Go via [Linker.c](#linkerc)
  - [ ] Java
  - [ ] Python
  - [ ] Ruby
  - [x] Rust via [Linker.c](#linkerc)

# Linker.c
Linker.c is a dynamic C-shared library linker that provides an interface for accessing any C-shared libraries. Accessing C, C++, Go, and Rust libraries has been covered in the usage section.

**Important Note**: Current version of Linker.c is only compatible with Linux-based operating systems.



## Usage 

### Install

```sh
sudo apt update && sudo apt -y install build-essential autoconf automake libtool
npm i @bitair/linker.c
```

### Sample

#### Node.js side

`index.js`

```js
const { link, ReturnType } = require('@bitair/linker.c')

const libPath = __dirname + '/lib.so'
const { add } = link(libPath, {
  add: ReturnType.Number
})

console.log(add(5, 3))
```

#### C side

`lib.c`

```c
double add(double val1, double val2) {
  return val1 + val2;
}
```

`build.sh`

```sh
gcc -c -fPIC lib.c && gcc -shared -o lib.so lib.o
```

#### C++ side

`lib.cpp`

```c
double cpp_add(double val1, double val2)
{
  return val1 + val2;
}

extern "C"
{
  double add(double val1, double val2)
  {
    return cpp_add(val1, val2);
  }
}
```

`build.sh`

```sh
gcc -c -fPIC lib.cpp && gcc -shared -o lib.so lib.o
```

#### Go side

`lib.go`

```go
package main

import "C"

func go_add(x float64, y float64) float64 {
	return x + y
}

//export add
func add(x C.double, y C.double) C.double {
	return C.double(go_add(float64(x), float64(y)))
}

func main() {}
```

`build.sh`

```sh
go build -o lib.so -buildmode=c-shared lib.go
```

#### Rust side

`lib.rs`

```rs
use libc::c_double;

fn rs_add(val1: f64, val2: f64) -> f64 {
    val1 + val2
}

#[no_mangle]
pub extern "C" fn add(val1: c_double, val2: c_double) -> c_double {
    return rs_add(val1, val2);
}
```

`Cargo.toml`

```
[lib]
crate-type = ["cdylib"]

[dependencies]
libc = "0.2"
```

`build.sh`

```sh
cargo build
```

Please see the [test folder](./libs/linker.c/test/) for a complete sample code. 


# API

```ts
link(libPath: string, functions: SignatureDict): Proxy
```

Links to a shared library and returns a proxy for accessing its functions.

- `libPath: string`

  Path of the shared library.

- `functions: SignatureDict`

  Signatures of the foreign functions. Note that the types of a function parameters would be inferred at the call time from the passing arguments.

```ts
type SignatureDict = {
  [key: string]: ReturnType
}
```

- `key: string`

  Name of the foreign function.

- `ReturnType`

  Type of the return value.

```ts
type Proxy = {
  [key: string]: (...params: unknown[]) => unknown
}
```

- `key: string`

  Name of the foreign function.

- `(...params:unknown[]) => unknown`

  The function's invoker.

```ts
enum ReturnType {
  ArrayBuffer,
  Boolean,
  Number,
  String
}
```

## Supported types and type mapping

| JS             | C             |
| -------------- | ------------- |
| ArrayBuffer    | struct Buffer |
| boolean        | bool          |
| number         | double        |
| string (UTF-8) | char \*       |

```c
typedef struct {
  void* data;
  size_t len;
} Buffer;
```

# License

[MIT License](./LICENSE)
