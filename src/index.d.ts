export declare function link(libPath: string, functions: SignatureDict): Proxy

export declare type SignatureDict = {
  [key: string]: ReturnType
}

export declare type Proxy = {
  [key: string]: (...params: unknown[]) => unknown
}

export declare enum ReturnType {
  ArrayBuffer,
  Boolean,
  Number,
  String
}