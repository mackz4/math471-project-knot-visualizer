/*
    WaterShallow.cpp implementation based off: https://github.com/hongkangcarl/PDE-solver-shallow-water-equations/blob/main/swe.py
*/

#include "WaterShallow.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

WaterShallow::WaterShallow()
{

}

void WaterShallow::init() {
    int timestep_curr = 0;
    b = 10.0f;   // friction coefficient
    g = 1.0f;    // gravity acceleration
    H = 10.0f;   // mean height of the surface

    linspace(xs, 0.0f, 1.0f, SIZE_X);
    linspace(ys, 0.0f, 1.0f, SIZE_Y);
    linspace(ts, 0.0f, 0.6f, TIMESTEPS); // tinker with value for "stop" and "TIMESTEPS" and the waves will stabalize instead of going crazy. 
                                          // in this instance, set to around "0.5f" and it will stabalize

    dx = xs[1] - xs[0];
    dy = ys[1] - ys[0];
    dt = ts[1] - ts[0];

    meshgrid(x, y, xs, ys);

    // initial condition : a single Gaussian pulse, no initial velocities
    float h_init[SIZE_X][SIZE_Y];
    float v0_init[SIZE_X][SIZE_Y];
    float v1_init[SIZE_X][SIZE_Y];

    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            h_init[i][j] = glm::exp((-glm::pow(x[i][j] - 0.2, 2) - glm::pow(y[i][j] - 0.2, 2)) / (2.0 * glm::pow(0.05, 2)));

        }
    }

    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            v0_init[i][j] = 0.0f;
            v1_init[i][j] = 0.0f;
        }
    }

    // store the initial condition into the lists
    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            h_list[0][i][j] = h_init[i][j];
            v0_list[0][i][j] = v0_init[i][j];
            v1_list[0][i][j] = v1_init[i][j];
        }
    }

    // start the simulation, for loop over the descritized time
    for (int i = 1; i < TIMESTEPS; i++) {
        // calculate the velocities based on the current h(x, y, t)
        float v0[SIZE_X][SIZE_Y];
        float v1[SIZE_X][SIZE_Y];
        for (int j = 0; j < SIZE_X; j++) {
            for (int k = 0; k < SIZE_Y; k++) {
                v0[j][k] = 0.0f;
                v1[j][k] = 0.0f;
            }
        }

        float h_list_last[SIZE_X][SIZE_Y];
        for (int j = 0; j < SIZE_X; j++) {
            for (int k = 0; k < SIZE_Y; k++) {
                h_list_last[j][k] = h_list[timestep_curr][j][k];
            }
        }
        float h_list_last_grad0[SIZE_X][SIZE_Y];
        float h_list_last_grad1[SIZE_X][SIZE_Y];

        grad(h_list_last_grad0, h_list_last_grad1, h_list_last, dx, dy);
        for (int j = 0; j < SIZE_X; j++) {
            for (int k = 0; k < SIZE_Y; k++) {
                v0[j][k] = (1.0f - dt * b) * v0_list[timestep_curr][j][k] - dt * g * h_list_last_grad0[j][k];
                v1[j][k] = (1.0f - dt * b) * v1_list[timestep_curr][j][k] - dt * g * h_list_last_grad1[j][k];
            }
        }

        // calculate the next - step h(x, y, t + ∆t) based on the velocities
        float h[SIZE_X][SIZE_Y];
        float v_div[SIZE_X][SIZE_Y];

        div(v_div, v0, v1);
        for (int j = 0; j < SIZE_X; j++) {
            for (int k = 0; k < SIZE_Y; k++) {
                h[j][k] = h_list[timestep_curr][j][k] - dt * H * v_div[j][k];
            }
        }


        float h_copy[SIZE_X][SIZE_Y];
        for (int j = 0; j < SIZE_X; j++) {
            for (int k = 0; k < SIZE_Y; k++) {
                h_copy[j][k] = h[j][k];
            }
        }
        // h[0, :] = h[1, :]
        for (int j = 0; j < SIZE_Y; j++) {
            h_copy[0][j] = h_copy[1][j];
        }

        // h[-1, :] = h[-2, :]
        for (int j = 0; j < SIZE_Y; j++) {
            h_copy[SIZE_X - 1][j] = h_copy[SIZE_X - 2][j];
        }

        // h[:, 0] = h[:, 1]
        for (int j = 0; j < SIZE_X; j++) {
            h_copy[j][0] = h_copy[j][1];
        }

        // h[:, -1] = h[:, -2]
        for (int j = 0; j < SIZE_X; j++) {
            h_copy[j][SIZE_Y - 1] = h_copy[j][SIZE_Y - 2];
        }
        
        // store the numerical results into the lists
        timestep_curr++;
        for (int j = 0; j < SIZE_X; j++) {
            for (int k = 0; k < SIZE_Y; k++) {
                h_list[timestep_curr][j][k] = h_copy[j][k];
            }
        }
        for (int j = 0; j < SIZE_X; j++) {
            for (int k = 0; k < SIZE_Y; k++) {
                v0_list[timestep_curr][j][k] = v0[j][k];
                v1_list[timestep_curr][j][k] = v1[j][k];
            }
        }
    }

    // PRINTING TEST
    for (int j = 0; j < SIZE_X; j++) {
        for (int k = 0; k < SIZE_Y; k++) {
            //std::cout << h_list[3][j][k] << std::endl;
        }
    }
    //std::cout << "DONE!" << std::endl;
    
}

void WaterShallow::linspace(float* arr, float start, float stop, int num) {
    float step = (stop - start) / (float)num;

    int index = 0;
    for (float i = start; i < stop; i+= step) {
        arr[index++] = i;
    }
}

void WaterShallow::meshgrid(float x[SIZE_X][SIZE_Y], float y[SIZE_X][SIZE_Y], float* xs, float* ys) {
    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            x[i][j] = xs[j];
            y[i][j] = ys[i];
        }
    }
}

void WaterShallow::grad(float h_list_last_grad0[SIZE_X][SIZE_Y], float h_list_last_grad1[SIZE_X][SIZE_Y], float h_list_last[SIZE_X][SIZE_Y], float dx, float dy) {
    // GRAD H X
    float np_roll_neg1_0[SIZE_X][SIZE_Y];
    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            if (i == 0) {
                np_roll_neg1_0[SIZE_X - 1][j] = h_list_last[i][j];
            }
            else {
                np_roll_neg1_0[i - 1][j] = h_list_last[i][j];
            }
        }
    }

    float np_roll_1_0[SIZE_X][SIZE_Y];
    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            if (i == SIZE_X - 1) {
                np_roll_1_0[0][j] = h_list_last[i][j];
            }
            else {
                np_roll_1_0[i + 1][j] = h_list_last[i][j];
            }
        }
    }

    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            h_list_last_grad0[i][j] = (np_roll_neg1_0[i][j] - np_roll_1_0[i][j]) / (2.0f * dx);
        }
    }

    for (int i = 0; i < SIZE_Y; i++) {
        h_list_last_grad0[0][i] = 0.0f;
        h_list_last_grad0[SIZE_X - 1][i] = 0.0f;
    }

    // GRAD H Y
    float np_roll_neg1_1[SIZE_X][SIZE_Y];
    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            if (j == 0) {
                np_roll_neg1_1[i][SIZE_Y - 1] = h_list_last[i][j];
            }
            else {
                np_roll_neg1_1[i][j - 1] = h_list_last[i][j];
            }
        }
    }

    float np_roll_1_1[SIZE_X][SIZE_Y];
    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            if (j == SIZE_Y - 1) {
                np_roll_1_1[i][0] = h_list_last[i][j];
            }
            else {
                np_roll_1_1[i][j + 1] = h_list_last[i][j];
            }
        }
    }

    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            h_list_last_grad1[i][j] = (np_roll_neg1_1[i][j] - np_roll_1_1[i][j]) / (2.0f * dy);
        }
    }

    for (int i = 0; i < SIZE_X; i++) {
        h_list_last_grad1[i][0] = 0.0f;
        h_list_last_grad1[i][SIZE_Y - 1] = 0.0f;
    }
}

void WaterShallow::div(float v_div[SIZE_X][SIZE_Y], float v0[SIZE_X][SIZE_Y], float v1[SIZE_X][SIZE_Y]) {
    float output0[SIZE_X][SIZE_Y];
    float output1[SIZE_X][SIZE_Y];

    for (int i = 0; i < SIZE_X; i++) {
        if (i == 0) {
            for (int j = 0; j < SIZE_X; j++) {
                output1[j][0] = (v1[j][1] - v1[j][0]) / 1.0;
            }
        }
        else if (i == SIZE_X - 1) {
            for (int j = 0; j < SIZE_X; j++) {
                output1[j][SIZE_X - 1] = (v1[j][SIZE_X - 1] - v1[j][SIZE_X - 2]) / 1.0;
            }
        }
        else {
            for (int j = 0; j < SIZE_X; j++) {
                output1[j][i] = (v1[j][i + 1] - v1[j][i - 1]) / 2.0;
            }
        }
    }


    for (int i = 0; i < SIZE_Y; i++) {
        if (i == 0) {
            for (int j = 0; j < SIZE_X; j++) {
                output0[0][j] = (v0[1][j] - v0[0][j]) / 1.0;
            }
        }
        else if (i == SIZE_Y - 1) {
            for (int j = 0; j < SIZE_X; j++) {
                output0[SIZE_Y - 1][j] = (v0[SIZE_Y - 1][j] - v0[SIZE_Y - 2][j]) / 1.0;
            }
        }
        else {
            for (int j = 0; j < SIZE_X; j++) {
                output0[i][j] = (v0[i + 1][j] - v0[i - 1][j]) / 2.0;
            }
        }
    }

    // Put together
    for (int i = 0; i < SIZE_X; i++) {
        for (int j = 0; j < SIZE_Y; j++) {
            v_div[i][j] = (output0[i][j] / dx) + (output1[i][j] / dy);
        }
    }
}

void WaterShallow::add_a_drop(float h) {


}