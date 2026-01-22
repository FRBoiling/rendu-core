#pragma once

#include "common/define.h"
#include "common/log/sink.h"
#include <fstream>
#include <string>

BEGIN_NAMESPACE_COMMON
namespace log {

class FileSink : public Sink {
public:
    enum class Rotation {
        None,
        Daily,
        Size
    };

    explicit FileSink(const std::string& filename, Level level = Level::Info);
    ~FileSink() override;

    void set_rotation(Rotation rotation, size_t max_size = 10 * 1024 * 1024); // 10MB

protected:
    void write(const std::string& formatted) override;

private:
    void rotate();
    std::string generate_filename(const std::string& base, int index);
    std::string get_current_date_string();

    std::string filename_;
    Rotation rotation_;
    size_t max_size_;
    size_t current_size_;
    std::ofstream file_;
    std::string last_date_;
};

} // namespace log
END_NAMESPACE_COMMON
