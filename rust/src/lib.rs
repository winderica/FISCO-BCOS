
mod air;
mod utils;
mod rescue;
use air::{build_trace, PublicInputs, RescueAir};
use std::ffi::CStr;
use std::ffi::CString;
use std::os::raw::c_char;
use winterfell::{
    math::{fields::f128::BaseElement, FieldElement},
    FieldExtension, HashFunction, ProofOptions, StarkProof,
};
use std::slice;
extern crate base64;

const LENGTH: usize = 1024;

#[no_mangle]
pub extern fn hash(value_ptr: *const u8, seed_ptr: *const u8) -> *mut u8 {
    let value_raw = unsafe { slice::from_raw_parts(value_ptr as *const u128, 2) };
    let seed_raw = unsafe { slice::from_raw_parts(seed_ptr as *const u128, 2) };
    let mut values = [seed_raw[0], seed_raw[1], value_raw[0], value_raw[1]].map(BaseElement::from);
    let mut result = [BaseElement::ZERO; 4];
    for _ in 0..LENGTH {
        rescue::hash(values, &mut result);
        values.copy_from_slice(&result);
    }
    Box::into_raw(Box::new(result)) as *mut _
}

#[no_mangle]
pub extern fn prove(value_ptr: *const u8, seed_ptr: *const u8, result_ptr: *const u8) -> *mut c_char {
    let value_raw = unsafe { slice::from_raw_parts(value_ptr as *const u128, 2) };
    let seed_raw = unsafe { slice::from_raw_parts(seed_ptr as *const u128, 2) };
    let result_raw = unsafe { slice::from_raw_parts(result_ptr as *const u128, 2) };
    let value = [BaseElement::from(value_raw[0]), BaseElement::from(value_raw[1])];
    let seed = [BaseElement::from(seed_raw[0]), BaseElement::from(seed_raw[1])];
    let result = [BaseElement::from(result_raw[0]), BaseElement::from(result_raw[1])];
    let trace = build_trace(value, seed, LENGTH);

    // generate the proof
    let pub_inputs = PublicInputs {
        seed,
        result,
    };
    let options = ProofOptions::new(
        42, // number of queries
        4,  // blowup factor
        16,  // grinding factor
        HashFunction::Blake3_256,
        FieldExtension::None,
        8,   // FRI folding factor
        256, // FRI max remainder length
    );
    CString::new(base64::encode(winterfell::prove::<RescueAir>(trace, pub_inputs, options).unwrap().to_bytes())).unwrap().into_raw()
}

#[no_mangle]
pub extern fn verify(seed_ptr: *const u8, result_ptr: *const u8, proof_ptr: *const c_char) -> bool {
    let seed_raw = unsafe { slice::from_raw_parts(seed_ptr as *const u128, 2) };
    let result_raw = unsafe { slice::from_raw_parts(result_ptr as *const u128, 2) };
    let proof_raw = unsafe { CStr::from_ptr(proof_ptr) };
    let seed = [BaseElement::from(seed_raw[0]), BaseElement::from(seed_raw[1])];
    let result = [BaseElement::from(result_raw[0]), BaseElement::from(result_raw[1])];
    let proof = base64::decode(proof_raw.to_bytes()).unwrap();
    let pub_inputs = PublicInputs {
        seed,
        result,
    };
    match winterfell::verify::<RescueAir>(StarkProof::from_bytes(&proof).unwrap(), pub_inputs) {
        Ok(_) => true,
        Err(_) => false,
    }
}

