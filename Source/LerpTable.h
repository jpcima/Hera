// SPDX-License-Identifier: ISC

#pragma once
#include <juce_core/juce_core.h>
#include <memory>
#include <functional>

class LerpTable {
public:
    LerpTable(const std::function<double(double)> &function,
              double min, double max, int numPoints)
        : numPoints_(numPoints), data_(new float[numPoints]),
          offset_(min), scale_((double)(numPoints - 1) / (max - min))
    {
        float *data = data_.get();
        for (int i = 0; i < numPoints; ++i)
            data[i] = function(min + (max - min) * i / (double)(numPoints - 1));
    }

    float operator()(float x) const noexcept
    {
        const float *data = data_.get();
        int numPoints = numPoints_;
        float index = juce::jlimit<float>(0, numPoints - 1, (x - offset_) * scale_);
        int i1 = (int)index;
        int i2 = juce::jmin(numPoints - 1, i1 + 1);
        float dx = index - i1;
        float y1 = data[i1];
        float y2 = data[i2];
        return y1 + dx * (y2 - y1);
    }

private:
    int numPoints_ = 0;
    std::unique_ptr<float[]> data_;
    float offset_ = 0;
    float scale_ = 0;
};
