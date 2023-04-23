const linker = require('bindings')('linker_c.node')

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

function link(libPath, functions) {
  const proxy = {}
  for (const key in functions) {
    if (Object.hasOwnProperty.call(functions, key)) {
      const returnType = functions[key]
      proxy[key] = (...params) => {
        const argTypes = params.map(p => getValueType(p))
        const result = linker.invoke(libPath, key, params, argTypes, returnType)
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
      else throw new Error(`Value type ${valueType} is not supported`)
  }
}

module.exports = {
  link,
  ReturnType
}
