# opp_with_transformer_cpp  

cource_classgen.cpp  

It makes the course class file(n-l.yaml) from cource line images(n-l.jpg)  

Save n.jpg image files from a part of Ros2 Global cost map.   
The robo moves from left center to right centr avoiding obstacle.  

![n.jpg file](https://github.com/tosa-no-onchan/opp_with_transformer_cpp/blob/main/images/1.jpg)  

With Gimp, drow a center line for robo mower move on it avoiding obstacle inflatuion and save n-l.jpg as cource line images.  
Line is 2 dot width.  
If no obstacle on center , the line becames a straight-line.   

![n-l.jpg file](https://github.com/tosa-no-onchan/opp_with_transformer_cpp/blob/main/images/1-l.jpg)  

Run cource_classgen to make cource class file n-l.yaml.  

``````
image: 1.jpg
cols: 178
rows: 122
x_interval: 4
data_num: 44
class_num: 49
data: 24 24 24 24 24 24 24 23 22 21 21 21 22 23 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 24 23 22 21 21 21 22
``````

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
