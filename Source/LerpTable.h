// SPDX-License-Identifier: ISC

#pragma once
#include <juce_core/juce_core.h>
#include <memory>
#include <functional>
#include <algorithm>

class LerpTable {
public:
    LerpTable(const std::function<double(double)> &function, double min, double max, int numPoints)
    {
        float *data = new float[numPoints];

        numPoints_ = numPoints;
        data_ = data;
        offset_ = min;
        scale_ = (double)(numPoints - 1) / (max - min);
        dataOwned_ = true;

        for (int i = 0; i < numPoints; ++i)
            data[i] = function(min + (max - min) * i / (double)(numPoints - 1));
    }

    template <class T>
    LerpTable(const T *data, double min, double max, int numPoints, bool copyData)
    {
        numPoints_ = numPoints;
        offset_ = min;
        scale_ = (double)(numPoints - 1) / (max - min);

        if (!copyData) {
            data_ = data;
            dataOwned_ = false;
        }
        else {
            float *dataCopy = new float[numPoints];
            std::copy(data, data + numPoints, dataCopy);
            data_ = dataCopy;
            dataOwned_ = true;
        }
    }

    template <class T, size_t N>
    LerpTable(const T (&data)[N], double min, double max, bool copyData)
        : LerpTable(data, min, max, N, copyData)
    {
    }

    ~LerpTable()
    {
        if (dataOwned_)
            delete[] data_;
    }

    float operator()(float x) const noexcept
    {
        const float *data = data_;
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
    const float *data_ = nullptr;
    float offset_ = 0;
    float scale_ = 0;
    bool dataOwned_ = false;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LerpTable)
};
