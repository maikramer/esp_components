//
// Created by maikeu on 25/11/22.
//

#ifndef ROTULADORAVENERA_STEPPER_H
#define ROTULADORAVENERA_STEPPER_H


#include <rom/ets_sys.h>
#include "Utility.h"
#include "Event.h"

constexpr uint32_t DefaultSpeed = 200;

class Stepper {
public:
    Stepper(gpio_num_t step, gpio_num_t direction, gpio_num_t enable) {
        Step = step;
        Direction = direction;
        Enable = enable;
    }

    void Move(uint32_t steps, uint32_t speed = DefaultSpeed);

    void Stop();

    void Init();

    gpio_num_t Step;
    gpio_num_t Direction;
    gpio_num_t Enable;
    uint32_t LastStepLevel = 0;
    bool IsMoving = false;
    uint32_t StepCount = 0;
    uint32_t StepsDesired = 0;
    esp_timer_handle_t Timer;
    NakedEvent OnFinishStepping;
private:

};


#endif //ROTULADORAVENERA_STEPPER_H
