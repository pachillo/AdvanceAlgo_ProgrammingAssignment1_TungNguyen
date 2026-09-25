(* Naive way to compute discrete fourier transform *)
let naive_dft (x : Complex.t array) : Complex.t array =
  (* Grab the length of the input array *)
  let n = Array.length x in
  let n_float = float_of_int n in (* Convert to float for division *)
  let pi = 4.0 *. atan 1.0 in     (* Compute the value of pi *)
  
  (* Create a new array length n *)
  (* For each number, the fucntion k is executed on it *)
  Array.init n (fun k ->
    let k_float = float_of_int k in
    (* Compute the summation *)
    let rec loop j acc =
      if j = n then acc
      else
        let j_float = float_of_int j in
        let angle = -2.0 *. pi *. k_float *. j_float /. n_float in

        (* Compute the complex number re^(iθ) *)
        let twiddle = Complex.polar 1.0 angle in

        (* Multiply x[j] by twiddle *)
        let term = Complex.mul x.(j) twiddle in
        loop (j + 1) (Complex.add acc term)
    in
    loop 0 Complex.zero)  (* Start the loop at j = 0 *)
