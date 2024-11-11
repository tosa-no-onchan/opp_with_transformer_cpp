# opp_with_transformer_cpp  

cource_classgen.cpp  
Make the course class text from cource line images  

#### 1. how to build  
$ git clone https://github.com/tosa-no-onchan/opp_with_transformer_cpp.git  

cd opp_with_transformer_cpp  
edit cource_classgen.cpp  

    std::string data_path="????/image";  
    genClassText.cont_f_ =false;

$ mkdir build  
$ cd build  
$ cmake ..  
$ make  

#### 2. how to run.  
$ ./cource_classgen  
