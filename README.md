This project used for encrypt && decrypt files.


g++ main.cpp -ggdb -std=c++11 -o a.out



#### test
./a.out  en_code  input_file_name    mid_file_cnt   new_output_name

#### encrypt
./a.out  en_code input_file_name  mid_file_cnt


#### decrypt
./a.out de_code output_file_name valid_size  max_file_index


文件加密、解密
 [文件名称]， 中间文件个数 
./a.out en_code aztec_small.rar 1


 [ 输出文件名称], [文件有效size]， [中间文件个数。]
./a.out de_code new.rar 463653 1
