

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <vector>
#include <assert.h>
#include <memory.h>
#include <cmath>
#include <sstream>
#include <iomanip>


using namespace std;


typedef int int128_t __attribute__((mode(TI)));
typedef unsigned int uint128_t __attribute__((mode(TI)));

char *in_buffer  = nullptr;
char *out_buffer = nullptr;

uint32_t valid_file_size = 0;//output arg

uint32_t max_file_index = 0;//output arg


char _4_bits_char_to_hex(char c){

        char res = '0';
        char offset = (c - 0);

        if(offset<10){
                res = '0' + offset;
        }
        else{
                offset = offset - 10;
                res = 'a' + offset;
        }
        return res;
}

std::string char_to_hex(char c){

        char high = ((c & 0xF0) >>4);
        char low  = (c & 0x0F);

        char h_res = _4_bits_char_to_hex(high);
        char l_res = _4_bits_char_to_hex(low);

        std::string res;
        res.push_back(h_res);
        res.push_back(l_res);
        return res;
}



std::ostream&
operator<<( std::ostream& dest, uint128_t value )
{
        std::ostream::sentry s( dest );
        if ( s ) {
                __uint128_t tmp = value < 0 ? -value : value;
                char buffer[ 128 ];
                char* d = std::end( buffer );
                do
                {
                        -- d;
                        *d = "0123456789"[ tmp % 10 ];
                        tmp /= 10;
                } while ( tmp != 0 );
                if ( value < 0 ) {
                        -- d;
                        *d = '-';
                }
                int len = std::end( buffer ) - d;
                if ( dest.rdbuf()->sputn( d, len ) != len ) {
                        dest.setstate( std::ios_base::badbit );
                }
        }
        return dest;
}


uint128_t stringToUint128(string s, bool &hasError){

        //cout<<s<<endl;

        hasError = false;
        //bytes memory b = bytes(s);
        std::string b = s;

        uint128_t result = 0;
        uint128_t oldResult = 0;
        for (uint128_t i = 0; i < b.length(); i++) { // c = b[i] was not needed
                if (b[i] >= 48 && b[i] <= 57) {
                        // store old value so we can check for overflows
                        oldResult = result;
                        result = result * 10 + (uint128_t(b[i]) - 48); // bytes and int are not compatible with the operator -.

                        //cout<< i <<":"<< result <<endl;
                        // prevent overflows
                        if(oldResult > result ){
                                // we can only get here if the result overflowed and is smaller than last stored value
                                assert(0 && "error: overflow!");
                                hasError = true;
                        }
                } else {
                        assert(0 && "error: bad char!");
                        hasError = true;
                }
        }
        return result;
}

uint32_t Trans(std::string in,uint32_t to_file_cnt){


        uint32_t mid_file_count = to_file_cnt;

        std::ifstream in_file(in.data(), std::ios::binary | std::ios::ate);
        std::streamsize t_valid_file_size = in_file.tellg();
        valid_file_size = t_valid_file_size;
        cout<<"valid_file_size = "<<valid_file_size<<endl;

        in_file.seekg(0, std::ios::beg);

        uint32_t buffer_size = valid_file_size + 32;//another 32 Byte for encode easy.
        in_buffer = new char[buffer_size];
        memset(in_buffer,0,sizeof(char)*buffer_size);

        if (in_file.read(in_buffer, valid_file_size))
        {
                    /* worked! */
                        cout<<(valid_file_size/(1024*1024))<<"MB"<<endl;

                        //total number of tokens.
                        //uint32_t total_count = ceil(((float)valid_file_size)/sizeof(uint128_t));

                        //encode to uint32_t
                        uint32_t total_count = ceil(((float)valid_file_size)/sizeof(char));

                        //how many token in a file.
                        uint32_t single_count = (total_count/mid_file_count);

                        cout<<"total_count = "<<total_count<<endl;
                        cout<<"single_count = "<<single_count<<endl;

                        std::string file_name_prefix = "mid";
                        for(uint32_t i =0;i<mid_file_count;i++){
                                std::string file_name = file_name_prefix+"_"+to_string(i)+".txt";
                                ofstream out_file(file_name.data(), std::ios::binary | std::ios::trunc);
                                out_file.flush();
                                out_file.close();
                        }


                        //encode to char
                        char *in_ptr = ((char *)in_buffer);


                        uint32_t pre_file_index = -1;
                        uint32_t write_cnt = 1;

                        for(uint32_t i=0; i<total_count; i++){
                                //encode to char
                                char data = (*in_ptr);

                                uint32_t file_index = i/single_count;

                                max_file_index = (max_file_index>file_index? max_file_index:file_index);


                                std::string file_name = file_name_prefix+"_"+to_string(file_index)+".txt";
                                ofstream out_file(file_name.data(), std::ios::binary | std::ios::app);


                                std::string hex_res = char_to_hex(data);
                                out_file<<hex_res;


                                if(write_cnt%512==0){
                                        out_file<<","<<endl;
                                }

                                write_cnt++;

                                out_file.flush();
                                out_file.close();

                                in_ptr++;
                        }
        }

        in_file.close();

        cout<<"valid_file_size = "<<valid_file_size<<endl;
        cout<<"valid_file_size = "<<valid_file_size<<endl;
        cout <<"max_file_index = "<<max_file_index<<endl;
        cout <<"max_file_index = "<<max_file_index<<endl;

        return 0;
}


char de_char_to_hex(char c){
        char arr[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

        char d=0;
        for(uint32_t i=0;i<16;i++){
                if(c==arr[i]){
                        d= i;
                        return d;
                }
        }
        assert(0&& "fail to find the c");
        return d;
}



uint32_t Re_Trans(std::string out, bool be_debug, uint32_t valid_file_size_arg, uint32_t max_file_index_arg){

        const uint32_t buffer_len = valid_file_size_arg + 32;
        out_buffer = new char [buffer_len];
        memset(out_buffer,0,sizeof(char)*buffer_len);

        char *out_ptr = ((char *)out_buffer);

        char *in_ptr  = nullptr;
        if(be_debug==true){
                in_ptr  = ((char *)in_buffer);
        }

        uint32_t write_index = 0;

        std::string file_name_prefix = "mid";
        for(uint32_t file_index =0; file_index<=max_file_index_arg; file_index++){
                std::string in_file_name = file_name_prefix+"_"+to_string(file_index)+".txt";
                std::ifstream in_file(in_file_name.data());

                ofstream out_file;
                if(be_debug==true){
                        std::string out_file_name = file_name_prefix+"_"+to_string(file_index)+"_t"+".txt";
                        out_file.open(out_file_name.data(), std::ios::binary | std::ios::trunc);
                }

                uint32_t line_idx=0;
                std::string line;
                while(std::getline(in_file,line)){
                        line_idx++;
                        if(be_debug==true){
                                out_file<<line<<endl;
                        }

                        std::stringstream ss(line);
                        std::string t_item;
                        std::vector<std::string> splittedStrings;
                        while (std::getline(ss, t_item, ','))
                        {
                                assert((t_item.size()%2==0)&& "expect even number of char.");
                                splittedStrings.push_back(t_item);
                        }

                        std::vector<std::string>::const_iterator ite;
                        for(ite= splittedStrings.begin(); ite!= splittedStrings.end();ite++){
                                std::string item = (*ite);

                                for(uint32_t i=0;i<item.length();i=i+2){

                                        char out_data = 0;
                                        bool error = false;
                                        //out_data = stringToUint128(item, error);
                                        //assert(error==false && "error: fail to trans!");

                                        char c0 = item[i+0];
                                        char c1 = item[i+1];

                                        char d0=de_char_to_hex(c0);
                                        char d1=de_char_to_hex(c1);

                                        out_data = (d0<<4)|(d1);


                                        if(be_debug==true){
                                                char in_data = in_ptr[write_index];
                                                if(in_data != out_data){
                                                        cout<<"Component compare failed at : "<<write_index<<endl;
                                                        assert(in_data == out_data && "error: data compare failed!");
                                                }
                                        }


                                        out_ptr[write_index] = out_data;

                                        if(be_debug==true){
                                                if(memcmp((out_ptr+write_index), (in_ptr+write_index), 1)!=0){
                                                        cout<<"write_index: "<<write_index<<endl;
                                                        assert(0 && "error: fail context error!");
                                                }
                                        }

                                        write_index++;
                                }
                        }
                }
                in_file.close();

                if(be_debug==true){
                        out_file.flush();
                        out_file.close();
                }

        }

        if(be_debug==true){
                if(memcmp(out_buffer, in_buffer,valid_file_size_arg) !=0){
                        for(uint32_t i =0;i<valid_file_size_arg;i++){
                                if(out_buffer[i]!=in_buffer[i]){
                                        cout<<i<<endl;
                                        assert(0 && "error:compare failed");
                                }
                        }
                }
        }

        std::ofstream outfile (out.data(),std::ofstream::binary|std::ios::trunc);
        outfile.write (out_buffer,valid_file_size_arg);
        outfile.flush();
        outfile.close();

        return 0;
}


int main(int argc, char **argv){
        //cout <<sizeof(unsigned long long)<<endl;
        //cout <<sizeof(uint128_t)<<endl;


        //cout << "You have entered " << argc
        //      << " arguments:" << "\n";

        //for (int i = 0; i < argc; ++i)
        //      cout << argv[i] << "\n";


        //assert(argc ==4 && "error: expect 4 arguments!");


        enum operation{
                TEST = 0,
                EN_CODE =1,
                DE_CODE =2,
                UNKNOWN = -1
        };
        operation op;

        std::string OP_TYPE =argv[1];
        if(OP_TYPE=="test"){
                op= TEST;
        }
        if(OP_TYPE=="en_code"){
                op= EN_CODE;
        }
        if(OP_TYPE=="de_code"){
                op= DE_CODE;
        }

        switch(op){
                case TEST:
                        {
                                assert(argc == 5 && "error: test need 5 arg.");
                                std::string IN =argv[2];
                                std::string mid_file_cnt =argv[3];
                                std::string OUT =argv[4];

                                uint32_t to_file_cnt = stoi(mid_file_cnt.data());
                                Trans(IN, to_file_cnt);
                                Re_Trans(OUT,true,valid_file_size,max_file_index);
                        }
                        break;
                case EN_CODE:
                        {
                                assert(argc == 4 && "error: en_code need 4 arg.");
                                std::string IN =argv[2];
                                std::string mid_file_cnt =argv[3];
                                uint32_t to_file_cnt = stoi(mid_file_cnt.data());
                                Trans(IN, to_file_cnt);
                        }
                        break;
                case DE_CODE:
                        {
                                assert(argc == 5 && "error: de_code need 5 arg.");

                                std::string OUT =argv[2];
                                std::string valid_size  =argv[3];
                                std::string max_index  =argv[4];
                                uint32_t valid_file_size_arg = stoi(valid_size.data());
                                uint32_t max_file_index_arg = stoi(max_index.data());

                                Re_Trans(OUT, false,  valid_file_size_arg, max_file_index_arg);
                        }
                        break;
                default:
                        assert(0 && "error: unknown operation!");
        }
        return 0;
}
