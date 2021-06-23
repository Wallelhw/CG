// #include<cmath>
// #include<eigen3/Eigen/Core>
// #include<eigen3/Eigen/Dense>
 #include<iostream>
 #include<thread>
// #include<random>
// #include<vector>
using namespace std;

// inline float get_random_float()
// {
//     std::random_device dev;
//     std::mt19937 rng(dev());
//     std::uniform_real_distribution<float> dist(0.f, 6.f); // distribution in range [1, 6]

//     return dist(rng);
// }
void proc()
{
    int a =9;
    cout<<"I'm subthread, para:"<<a<<endl;
    cout<<"current thread id:"<<this_thread::get_id()<<endl;
}

int main(){

    cout<<"I'm main thread"<<endl;
    int a = 8;
    std::thread th2 = std::thread(proc);
    cout<<"show subtheard id:"<<th2.get_id()<<" at main thread"<<endl;
    th2.join();
    return 0;
    // std::random_device dev;
    // std::mt19937 rng(dev());
    // std::uniform_real_distribution<float> dist(0.f, 1.f);
    
    // for(int i =0;i< 10;++i){
    //     float res = get_random_float();
    //     std::cout<< rng() <<std::endl;
    // }
    // Basic Example of cpp
    // std::cout << "Example of cpp \n";
    // float a = 1.0, b = 2.0;
    // std::cout << a << std::endl;j  
    // std::cout << a/b << std::endl;
    // std::cout << std::sqrt(b) << std::endl;
    // std::cout << std::acos(-1) << std::endl;
    // std::cout << std::sin(30.0/180.0*acos(-1)) << std::endl;

    // // Example of vector
    // std::cout << "Example of vector \n";
    // // vector definition
    // Eigen::Vector3f v(1.0f,2.0f,3.0f);
    // Eigen::Vector3f w(1.0f,0.0f,0.0f);
    // // vector output
    // std::cout << "Example of output \n";
    // std::cout << v << std::endl;
    // // vector add
    // std::cout << "Example of add \n";
    // std::cout << v + w << std::endl;
    // // vector scalar multiply
    // std::cout << "Example of scalar multiply \n";
    // std::cout << v * 3.0f << std::endl;
    // std::cout << 2.0f * v << std::endl;

    // // Example of matrix
    // std::cout << "Example of matrix \n";
    // // matrix definition
    // Eigen::Matrix3f i,j;
    // i << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
    // j << 2.0, 3.0, 1.0, 4.0, 6.0, 5.0, 9.0, 7.0, 8.0;
    // // matrix output
    // std::cout << "Example of output \n";
    // std::cout << i << std::endl;
    // // matrix add i + j
    // matrix scalar multiply i * 2.0
    // matrix multiply i * j
    // matrix multiply vector i * v


    /* 
    * PA 0
    */
    // TO DO: Define point P
    // TO DO: Define rotation matrix M
    // TO DO: M * P
    return 0;
}