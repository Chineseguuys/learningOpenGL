#ifndef __ATTENUATION__
#define __ATTENUATION__

struct AttenuationValue {
public:
    int distance;
    float constants;
    float linear;
    float quadratic;

public:
    AttenuationValue(int _dis, float _cons, float _line, float _quad) {
        this->distance = _dis;
        this->constants = _cons;
        this->linear = _line;
        this->quadratic = _quad;
    }
};


// 部分定义
AttenuationValue attenuation_7 = {7, 1.0, 0.7, 1.8};
AttenuationValue attenuation_13 = {13, 1.0, 0.35, 0.44};
AttenuationValue attenuation_20 = {20, 1.0, 0.22, 0.20};
AttenuationValue attenuation_32 = {32, 1.0, 0.14, 0.07};
AttenuationValue attenuation_50 = {50, 1.0, 0.09, 0.032};
AttenuationValue attenuation_65 = {65, 1.0, 0.07, 0.017};
AttenuationValue attenuation_100 = {100, 1.0, 0.045, 0.0075};
AttenuationValue attenuation_160 = {160, 1.0, 0.027, 0.0028};
AttenuationValue attenuation_200 = {200, 1.0, 0.022, 0.0019};
AttenuationValue attenuation_325 = {325, 1.0, 0.014, 0.0007};


#endif  /* __ATTENUATION__ */