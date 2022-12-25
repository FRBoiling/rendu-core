#ifndef RENDU_UTILS_FORMAT_H_
#define RENDU_UTILS_FORMAT_H_

namespace rendu::utils
{
    template <typename... Args>
    [[nodiscard]] fmt::string Format(const std::string_view msg, Args&&... args) noexcept
    {
        try {
            if(!msg.empty()) {
                return fmt::vformat(msg, fmt::make_format_args(std::forward<Args>(args)...));
            }
        } catch(const fmt::format_error&) {
            Log::Console<Log::Exception>(
                "\n----------------------------------------\n"
                "|| Error: format syntax invalid!\n"
                "|| Format: {}"
                "\n----------------------------------------", msg);
        } catch(const std::bad_alloc&) {
            Log::Console<Log::Exception>(
                "\n----------------------------------------\n"
                "|| Error: not enough memory for alloc\n"
                "|| Format: {}"
                "\n----------------------------------------", msg);
        }

        return std::string{};
    }
}

#endif // RENDU_UTILS_FORMAT_H_