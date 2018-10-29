# PPM-tools
Tools for compression, decompression, mirror and overlay PPM(portable pixmap format) images.

### Installation

```
git clone https://github.com/andrei-stratila/PPM-tools.git
make 
```
### How it works!

  General command: 
  ./ppm-tools [-c ratio | -d | -m type | -o] [input_file_1] [input_file_2](only for -o) [output_file]
  
*  -c grade = Compress the input_file_1 having image compression ratio [ratio];
*  -d = Decompress the input_file_1;
*  -m type = Mirror the input_file_1, [type] could be v(vertically) or h(horizontally). There could be an extra argument: [ratio] for image compression ratio;
*  -o = Overlay the input_file_1 and input_file_2 images. There could be an extra argument: [ratio] for image compression ratio.

### Examples

  For the next examples follow the files in examples directory.
  
>  Compression examples:
```
Command 1: ./ppm-tools -c 0 test0.ppm compress0_0.out
Output file: compress0_0.out

Command 2: ./ppm-tools -c 370 test1.ppm compress1_370.out
Output file: compress1_370.out
```
>  Decompression example:
```  
Command 3: ./ppm-tools -d compress1_370.out decompress1.ppm
Output file: decompress1.ppm
```
>  Mirror examples:
 ``` 
Command 4: ./ppm-tools -m h 0 test0.ppm mirror0_h_0.ppm
Output file: mirror0_h_0.ppm

Command 5: ./ppm-tools -m v 0 test0.ppm mirror0_v_0.ppm
Output file: mirror0_v_0.ppm

Command 6: ./ppm-tools -m h 370 test1.ppm mirror1_h_370.ppm
Output file: mirror1_h_370.ppm

Command 7: ./ppm-tools -m v 370 test1.ppm mirror1_v_370.ppm
Output file: mirror1_v_370.ppm
```
>  Overlay example:
 ``` 
Command 8: ./ppm-tools -o 100 test0.ppm test1.ppm overlay_100_test0_test1.ppm
Output file: overlay_100_test0_test1.ppm
```
