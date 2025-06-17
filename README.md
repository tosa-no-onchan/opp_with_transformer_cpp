# cource_classgen.cpp  

It makes the course class file(n-l.yaml) from cource line images(n-l.jpg)  

Save n.jpg image files from a part of Ros2 Global cost map.   
The robo moves from left center to right centr avoiding obstacle.  

Usual, run C++ Auto Mower with ml_data:=True  
$ ros2 launch turtlebot3_navi_my go_auto_mower.launch.py use_sim_time:=[True] ml_data:=True

This images size 122 x Max 600 with map resolution 0.025  
Usually, ROS2 Global costmap resolution is 0.05 , then it's original images size is 61 x Max 300  

![n.jpg file](https://github.com/tosa-no-onchan/opp_with_transformer_cpp/blob/main/images/1.jpg)  

With Gimp, drow a center line for robo mower move on it avoiding obstacle inflatuion and save n-l.jpg as cource line images.  
Line is 2 dot width.  
On Gimp, you had better to add a lines layer and white layer avobe or below original layer.  
Drow a center black line with 2 dot pencil on a lines layer.  
If no obstacle is on center , the line becames a straight-line.   
At last,export lines layer and white layer as to n-l.jpg and save all data to n.xcf(gimp original format file).  

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
Then, You can run opp_with_lstm training or opp_with_transformer_mltu trainning.  

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

#### 3. test data of mine  

[test data](https://huggingface.co/datasets/tosa-no-onchan/opp)  
How to get  
``````
$ source kivy_env-3.12/bin/activate
(kivy_env-3.12) $ python -m pip install datasets
(kivy_env-3.12) $ python
>>> from datasets import load_dataset
>>> dataset = load_dataset("tosa-no-onchan/opp",split="train")
image.zip: 100%|███████████████████████████| 24.4M/24.4M [00:08<00:00, 2.82MB/s]
Generating train split: 100%|██████| 2185/2185 [00:00<00:00, 6608.29 examples/s]
>>> dataset
Dataset({
    features: ['image'],
    num_rows: 2185
})
>>> exit()

``````
