# Intro

Linker.js is a foreign function interface for Node.js. Linker.js is part of [Concurrent.js](https://github.com/bitair-org/concurrent.js).

**Important Note**: Current version of Linker.js is only compatible with Linux-based operating systems.

# Features

- Language support
  - [x] C, C++, Go, Rust or any language that can produce C-shared libraries (via [C Linker](#c-linker))
  - [ ] Java
  - [x] Python (via [Py Linker](#py-linker))
  - [ ] Ruby

# Technical facts
  - Build upon the [Node-API](https://nodejs.org/api/n-api.html), [libffi](https://github.com/libffi/libffi) and [Python C-API](https://docs.python.org/3/c-api) libraries
  - Supports zero-copy data passing through buffers
  - Automatically collects the buffers memories
  - Has a minimal API


### Install

```sh
sudo apt -y install build-essential cmake
npm i -D cmake-js
npm i @bitair/linker.js
```

# C Linker

C Linker provides the interface for accessing any C-shared libraries. Accessing C, C++, Go, and Rust libraries has been covered in the usage section.

## Usage

### Install

```sh
sudo apt -y install libffi-dev
npx linker.js install c_linker
```

### Sample

#### Node.js side

`index.js`

```js
const { link, ReturnType } = require('@bitair/linker.js')

const libPath = __dirname + '/lib.so'
const { add } = link(libPath, {
  add: ReturnType.Number
})

console.log(add(5, 3))
```

#### C side

`lib.c`

```c
double add(double a, double b) {
  return a + b;
}
```

`build.sh`

```sh
gcc -c -fPIC lib.c && gcc -shared -o lib.so lib.o
```

#### C++ side

`lib.cpp`

```c
double cpp_add(double a, double b)
{
  return a + b;
}

extern "C"
{
  double add(double a, double b)
  {
    return cpp_add(a, b);
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

fn rs_add(a: f64, b: f64) -> f64 {
    a + b
}

#[no_mangle]
pub extern "C" fn add(a: c_double, b: c_double) -> c_double {
    return rs_add(a, b);
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

Please see the [test folder](./test/c_linker) for a complete sample code.


# Py Linker
Py Linker provides the interface for accessing python libraries.

## Usage

### Install

```sh
sudo apt -y install python3-dev
npx linker.js install py_linker
```

### Sample

#### Node.js side

`index.js`

```js
const { link, ReturnType } = require('@bitair/linker.js')

const libPath = __dirname + '/lib.py'
const { add } = link(libPath, {
  add: ReturnType.Number
})

console.log(add(5, 3))
```

#### Python side

`lib.py`

```python
def add(a, b):
  return float(a + b)
```

Please see the [test folder](./test/py_linker) for a complete sample code.

# API

```ts
link(libPath: string, functions: SignatureDict): Proxy
```

Links to a shared library and returns a proxy for accessing its functions.

- `libPath: string`

  Path of the shared library.

- `functions: SignatureDict`

  Signatures of the foreign functions. Note that the types of a function parameters will be inferred at the call time from the passing arguments.

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
      enum ReturnType {
        ArrayBuffer,
        Boolean,
        Number,
        String
      }
      ```

- `Proxy`  
  ```ts
  type Proxy = {
    [key: string]: (...params: unknown[]) => unknown
  }
  ```

    - `key: string`

      Name of the foreign function.

    - `(...params:unknown[]) => unknown`

      The function's invoker.

## Type mapping

| JS             | C             | Python     |
| -------------- | ------------- | ---------- |
| ArrayBuffer    | struct Buffer | memoryview |
| boolean        | bool          | bool       |
| number         | double        | float      |
| string (UTF-8) | char *        | str        |

```c
typedef struct {
  void* data;
  size_t len;
} Buffer;
```

Notes:
  - Strings, booleans, and numbers are shared by value between JavaScript and a foreign function. 
  - ArrayBuffers/Buffers are shared by reference. Their allocated memories will be automatically released once their JavaScript consumer is out of scope.
  - A string that's returned from a C function must be dynamically allocated. The allocated memory will be automatically released once the string has been passed to JavaScript.

# License

[MIT License](./LICENSE)
