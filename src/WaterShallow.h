#ifndef WATER_H
#define WATER_H

#include <glm/glm.hpp>

#define SIZE_X 51
#define SIZE_Y 51

class WaterShallow
{
public:
    WaterShallow();
    void init();
    void solve();

    float h_list[SIZE_X][SIZE_Y];
    float v0_list[SIZE_X][SIZE_Y];
    float v1_list[SIZE_X][SIZE_Y];
private:
    float b;
    float g;
    float H;

    float xs[SIZE_X];
    float ys[SIZE_Y];

    float dx;
    float dy;
    float dt;

    float x[SIZE_X][SIZE_Y];
    float y[SIZE_X][SIZE_Y];

    void linspace(float* arr, float start, float stop, int num);
    void meshgrid(float x[SIZE_X][SIZE_Y], float y[SIZE_X][SIZE_Y], float* xs, float* ys);
    void grad(float h_list_last_grad0[SIZE_X][SIZE_Y], float h_list_last_grad1[SIZE_X][SIZE_Y], float h_list_last[SIZE_X][SIZE_Y], float dx, float dy);
    void div(float v_div[SIZE_X][SIZE_Y], float v0[SIZE_X][SIZE_Y], float v1[SIZE_X][SIZE_Y]);
    void add_a_drop(float h);
};
#endif