let rec cooley_tukey (x : Complex.t array) : Complex.t array =
  let n = Array.length x in 
  let n_float = float_of_int n in 
  let pi = 4.0 *. atan 1.0 in

  if n <= 1 then 
    x 
  else

  let half = n / 2 in 
  let even_samples = Array.init half (fun i -> x.(2 * i)) in 
  let odd_samples = Array.init half (fun i -> x.(2 * i + 1)) in 

  let e = cooley_tukey even_samples in 
  let o = cooley_tukey odd_samples in

  let res = Array.make n Complex.zero in
    for k = 0 to half - 1 do
      let theta = -2.0 *. pi *. (float_of_int k) /. n_float in
      let w = { Complex.re = cos theta; Complex.im = sin theta } in
      let t = Complex.mul w o.(k) in

      res.(k)        <- Complex.add e.(k) t;
      res.(k + half) <- Complex.sub e.(k) t
    done;

    res










