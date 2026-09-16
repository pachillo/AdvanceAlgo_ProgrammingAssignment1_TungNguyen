open Fft_lib

let print_complex_array (label : string) (arr : Complex.t array) : unit =
  Printf.printf "%s:\n" label;
  Array.iteri
    (fun i c ->
      let sign = if c.Complex.im >= 0.0 then "+" else "-" in
      Printf.printf "  [%d] %.4f %s %.4fi\n" i c.Complex.re sign (abs_float c.Complex.im))
    arr;
  print_newline ()

let () =
  Printf.printf "========================================\n";
  Printf.printf " Advanced Algorithms - PA1 OCaml Setup \n";
  Printf.printf "========================================\n\n";

  let input =
    [|
      Complex.{ re = 1.0; im = 0.0 };
      Complex.{ re = 2.0; im = 0.0 };
      Complex.{ re = 3.0; im = 0.0 };
      Complex.{ re = 4.0; im = 0.0 };
    |]
  in

  print_complex_array "Input Signal" input;

  let freq = Naive_dft.naive_dft input in
  print_complex_array "DFT Result (Frequency Domain)" freq;

  Printf.printf "Run successful!\n"
