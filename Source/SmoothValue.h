// SPDX-License-Identifier: ISC

#pragma once
#include <cmath>

class OnePoleSmoothValue {
public:
    explicit OnePoleSmoothValue(float initialValue = 0)
        : currentValue(initialValue), targetValue(initialValue)
    {
        updatePole();
    }

    float getCurrentValue() const
    {
        return currentValue;
    }

    float getTargetValue() const
    {
        return targetValue;
    }

    void setTargetValue(float newValue)
    {
        targetValue = newValue;
    }

    void setCurrentAndTargetValue(float newValue)
    {
        currentValue = newValue;
        targetValue = newValue;
    }

    void setSampleRate(float newRate)
    {
        sampleRate = newRate;
        updatePole();
    }

    void setTimeConstant(float newTimeConstant)
    {
        timeConstant = newTimeConstant;
        updatePole();
    }

    float getNextValue()
    {
        return (currentValue = currentValue * pole + targetValue * (1 - pole));
    }

private:
    void updatePole()
    {
        if (timeConstant > 0)
            pole = std::exp(float(-2.0 * M_PI) / (timeConstant * sampleRate));
        else
            pole = 0;
    }

private:
    float currentValue = 0;
    float targetValue = 0;
    float pole = 0;
    float timeConstant = 0;
    float sampleRate = 44100;
};
