use libc::c_double;

fn rs_add(a: f64, b: f64) -> f64 {
    a + b
}

#[no_mangle]
pub extern "C" fn add(a: c_double, b: c_double) -> c_double {
    return rs_add(a, b);
}
