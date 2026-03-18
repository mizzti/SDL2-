#ifndef RANDOM_H
#define RANDOM_H

#include <random>

class Random
{
public:
    static Random& getInstance()
    {
        static Random instance;
        return instance;
    }

    float getFloat(float min = 0.0f, float max = 1.f)
    {
        std::uniform_real_distribution<float> dis(min, max);// 指定分布
        return dis(gen);
    }

private:
    // 生成随机数引擎
    std::mt19937 gen;
    Random() : gen(std::random_device{}()) {}// [NOTE] rd()：重载了(）,可以直接进行调用-
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;
};

#endif