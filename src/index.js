const bindings = require('bindings')

const ArgType = {
  ArrayBuffer: 0,
  Boolean: 1,
  Number: 2,
  String: 3
}

const ReturnType = {
  ArrayBuffer: 0,
  Boolean: 1,
  Number: 2,
  String: 3
}

const LIB_PATH_PATTERN = /(.*\/)(.+)/

function link(libPath, functions) {
  const proxy = {}
  for (const key in functions) {
    if (Object.hasOwnProperty.call(functions, key)) {
      const returnType = functions[key]
      proxy[key] = (...params) => {
        const argTypes = params.map(p => getValueType(p))

        let linker
        let [, libDir, libName] = libPath.match(LIB_PATH_PATTERN)

        if (libName.endsWith('.so')) {
          linker = bindings('c_linker.node')
        } else if (libName.endsWith('.py')) {
          libName = libName.replace('.py', '')
          linker = bindings('py_linker.node')
        } else {
          throw new Error(
            `Linker.js Error: Cannot link the library '%{libPath}'. Only '.so' and '.py' libraries can be linked.`
          )
        }

        let result = linker.invoke(libDir, libName, key, params, argTypes, returnType)
        if (returnType === ReturnType.Boolean) result = result === 1
        return result
      }
    }
  }
  return proxy
}

function getValueType(value) {
  const valueType = typeof value
  switch (typeof value) {
    case 'boolean':
      return ArgType.Boolean
    case 'number':
      return ArgType.Number
    case 'string':
      return ArgType.String
    default:
      if (valueType === 'object' && value instanceof ArrayBuffer) return ArgType.ArrayBuffer
      else throw new Error(`Argument type ${valueType} is not supported`)
  }
}

module.exports = {
  link,
  ReturnType
}
