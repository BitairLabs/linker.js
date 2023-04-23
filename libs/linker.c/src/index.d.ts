export declare enum ReturnType {
  ArrayBuffer,
  Boolean,
  Number,
  String
}

export declare function link(libPath: string, functions: SignatureDict): Proxy

declare type SignatureDict = {
  [key: string]: ReturnType
}

declare type Proxy = {
  [key: string]: (...params: unknown[]) => unknown
}

export { link, ReturnType }
