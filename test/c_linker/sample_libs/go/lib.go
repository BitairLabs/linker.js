package main

import "C"

func go_add(a float64, b float64) float64 {
	return a + b
}

//export add
func add(a C.double, b C.double) C.double {
	return C.double(go_add(float64(a), float64(b)))
}

func main() {}
