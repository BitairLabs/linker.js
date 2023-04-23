const { equal, deepEqual } = require('node:assert')
const { link, ReturnType } = require('../src/index')

const cLibPath = __dirname + '/../build/sample_libs/c/lib.so'
const cLib = link(cLibPath, {
  hello: ReturnType.String,
  add: ReturnType.Number,
  is_even: ReturnType.Boolean,
  is_true: ReturnType.Boolean,
  str_len: ReturnType.Number,
  str_join: ReturnType.String,
  max: ReturnType.Number,
  min: ReturnType.Number,
  prime_numbers: ReturnType.ArrayBuffer
})
equal(cLib.hello(), 'Hello world!')
equal(cLib.add(5, 3), 8)
equal(cLib.is_even(5), false)
equal(cLib.is_even(2), true)
equal(cLib.is_true(true), true)
equal(cLib.is_true(false), false)
equal(cLib.str_len('Hello world!'), 12)
equal(cLib.str_join('Hello', ' world!'), 'Hello world!')
equal(cLib.max(new Uint16Array([0, 1, 10, 100, 1000, 10000]).buffer), 10000)
equal(cLib.min(new Uint8Array([1, 2, 3, 4, 5]).buffer), 1)
deepEqual(Array.from(new Uint16Array(cLib.prime_numbers(10))), [2, 3, 5, 7])


const cppLibPath = __dirname + '/../build/sample_libs/cpp/lib.so'
const cppLib = link(cppLibPath, {
  add: ReturnType.Number
})
equal(cppLib.add(5, 3), 8)


const rustLibPath = __dirname + '/../build/sample_libs/rust/release/libsample.so'
const rustLib = link(rustLibPath, {
  add: ReturnType.Number
})
equal(rustLib.add(5, 3), 8)

const goLibPath = __dirname + '/../build/sample_libs/go/lib.so'
const goLib = link(goLibPath, {
  add: ReturnType.Number
})
equal(goLib.add(5, 3), 8)

console.log('Done!')
