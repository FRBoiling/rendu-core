#ifndef RENDU_TYPES_BENCHMARKSCOPED_H_
#define RENDU_TYPES_BENCHMARKSCOPED_H_


#include <cstdint>
#include <chrono>
#include "basic_loggers_def.h"
#include "source_location.h"

namespace rendu::types
{
    class BenchmarkScoped
    {
        using Timer = std::chrono::steady_clock;

        struct Benchmark {
            [[nodiscard]] static constexpr auto GetPrefix() noexcept {
                return Log::CreatePrefix("[Benchmark:");
            }

            [[nodiscard]] static constexpr auto GetStyle() noexcept {
                return Log::CreateStyle(Log::Color::BrightMagenta);
            }
        };

    public:
        BenchmarkScoped(const SourceLocation& location = SourceLocation::Create()) : m_Location{location}, m_Time{Timer::now()} {}
        ~BenchmarkScoped() {
            const std::chrono::duration<float> timeleft = Timer::now() - m_Time;
            const auto formater = Log::Formater<Benchmark>{"{}] Timeleft: {:.6f} sec", m_Location};
            Log::Console<Benchmark>(formater, m_Location.GetFunction(), timeleft.count());
        }
        BenchmarkScoped(const BenchmarkScoped&) = delete;
        BenchmarkScoped(BenchmarkScoped&&) noexcept = delete;
        BenchmarkScoped& operator=(const BenchmarkScoped&) = delete;
        BenchmarkScoped& operator=(BenchmarkScoped&&) noexcept = delete;
    private:
        SourceLocation m_Location;
        Timer::time_point m_Time;
    };
}

#endif // RENDU_TYPES_BENCHMARKSCOPED_H_