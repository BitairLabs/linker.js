const { equal, deepEqual } = require('node:assert')
const { link, ReturnType } = require('../../src/index')

const pyLibPath = __dirname + '/lib.py'
const pyLib = link(pyLibPath, {
  hello: ReturnType.String,
  add: ReturnType.Number,
  is_even: ReturnType.Boolean,
  is_true: ReturnType.Boolean,
  str_len: ReturnType.Number,
  str_join: ReturnType.String,
  get_max: ReturnType.Number,
  get_min: ReturnType.Number,
  prime_numbers: ReturnType.ArrayBuffer,
  sum: ReturnType.Number
})

equal(pyLib.add(5, 3), 8)
equal(pyLib.is_even(5), false)
equal(pyLib.is_even(2), true)
equal(pyLib.is_true(true), true)
equal(pyLib.is_true(false), false)
equal(pyLib.hello(), 'Hello world!')
equal(pyLib.str_len('Hello world!'), 12)
equal(pyLib.str_join('Hello', ' world!'), 'Hello world!')
equal(pyLib.get_max(new Uint16Array([0, 1, 10, 100, 1000, 10000]).buffer), 10000)
equal(pyLib.get_min(new Uint8Array([1, 2, 3, 4, 5]).buffer), 1)
deepEqual(Array.from(new Uint16Array(pyLib.prime_numbers(10))), [2, 3, 5, 7])
equal(pyLib.sum(new Uint8Array([1, 2, 3, 4]).buffer), 10)

console.log('Done!')
