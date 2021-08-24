#include "utility/LinearSmoother.h"
#include <benchmark/benchmark.h>

class LinearSmootherFixture : public benchmark::Fixture
{
public:
    LinearSmootherFixture()
    {
        smoother_.setSampleRate(44100);
        smoother_.setTimeConstant(1.0f);
        smoother_.setTarget(1.0f);
    }

    void SetUp(const benchmark::State &state)
    {
        smoother_.clear();
        output_.resize(state.range(0));
    }

    void TearDown(const ::benchmark::State &state)
    {
        (void)state;
    }

    LinearSmoother smoother_;
    std::vector<float> output_;
};

BENCHMARK_DEFINE_F(LinearSmootherFixture, ProcessSingle)(benchmark::State &state)
{
    for (auto _ : state)
    {
        LinearSmoother &smoother = smoother_;
        float *output = output_.data();
        uint32_t count = output_.size();
        for (uint32_t i = 0; i < count; ++i)
            output[i] = smoother.next();
    }
}

BENCHMARK_DEFINE_F(LinearSmootherFixture, ProcessBlock)(benchmark::State &state)
{
    for (auto _ : state)
    {
        smoother_.nextBlock(output_.data(), output_.size());
    }
}

BENCHMARK_REGISTER_F(LinearSmootherFixture, ProcessSingle)->RangeMultiplier(2)->Range(1 << 4, 1 << 16);
BENCHMARK_REGISTER_F(LinearSmootherFixture, ProcessBlock)->RangeMultiplier(2)->Range(1 << 4, 1 << 16);
BENCHMARK_MAIN();
