#include "common/config/watcher.h"
#include "common/config/loader.h"
#include "common/log/logger.h"
#include <filesystem>

#include "common/config/json_loader.h"

BEGIN_NAMESPACE_COMMON
    namespace config {

ConfigWatcher::ConfigWatcher(
    IoContext& io_context,
    const std::string& file_path,
    std::chrono::milliseconds check_interval)
    : io_context_(io_context)
    , file_path_(file_path)
    , check_interval_(check_interval)
    , running_(false)
    , last_mod_time_(std::nullopt) {

    JsonLoader loader;
    auto result = loader.load(file_path_);
    if (std::holds_alternative<Error>(result)) {
        RENDU_LOG_WARN("Initial config load failed: {}", std::get<Error>(result).message());
    } else {
        current_config_ = std::get<Config>(result);
        last_mod_time_ = get_file_modification_time();
    }
}

ConfigWatcher::~ConfigWatcher() {
    stop();
}

ConfigWatcher::ConfigWatcher(ConfigWatcher&& other) noexcept
    : io_context_(other.io_context_)
    , file_path_(std::move(other.file_path_))
    , check_interval_(other.check_interval_)
    , callbacks_(std::move(other.callbacks_))
    , running_(other.running_.load())
    , last_mod_time_(other.last_mod_time_)
    , current_config_(std::move(other.current_config_))
    , timer_(std::move(other.timer_)) {
    other.running_ = false;
}

void ConfigWatcher::start() {
    if (running_.load()) {
        RENDU_LOG_WARN("ConfigWatcher is already running");
        return;
    }

    if (!file_exists()) {
        RENDU_LOG_ERROR("Config file does not exist: {}", file_path_);
        return;
    }

    running_.store(true);
    timer_ = std::make_unique<io::Timer>(io_context_);
    timer_->repeat(check_interval_, [this]() {
        this->watch_loop();
    });
    RENDU_LOG_INFO("ConfigWatcher started, monitoring: {}", file_path_);
}

void ConfigWatcher::stop() {
    if (!running_.load()) {
        return;
    }
    running_.store(false);
    if (timer_) {
        timer_->cancel();
        timer_.reset();
    }
    RENDU_LOG_INFO("ConfigWatcher stopped");
}

void ConfigWatcher::on_change(Callback callback) {
    callbacks_.push_back(std::move(callback));
}

Result<config::Config> ConfigWatcher::reload() {
    if (!file_exists()) {
        return Error(ErrorCode::NotFound, "Config file does not exist: " + file_path_);
    }

    JsonLoader loader;
    auto result = loader.load(file_path_);
    if (std::holds_alternative<Error>(result)) {
        RENDU_LOG_ERROR("Config reload failed: {}", std::get<Error>(result).message());
        return result;
    }

    auto new_config = std::get<config::Config>(result);
    current_config_ = new_config;
    last_mod_time_ = get_file_modification_time();

    RENDU_LOG_INFO("Config reloaded successfully: {}", file_path_);

    for (const auto& callback : callbacks_) {
        try {
            callback(current_config_);
        } catch (const std::exception& e) {
            RENDU_LOG_ERROR("Config change callback exception: {}", e.what());
        } catch (...) {
            RENDU_LOG_ERROR("Config change callback unknown exception");
        }
    }

    return new_config;
}

bool ConfigWatcher::is_running() const {
    return running_.load();
}

const std::string& ConfigWatcher::file_path() const {
    return file_path_;
}

const config::Config& ConfigWatcher::config() const {
    return current_config_;
}

void ConfigWatcher::watch_loop() {
    if (!running_.load()) {
        return;
    }

    auto current_mod_time = get_file_modification_time();
    if (!current_mod_time.has_value()) {
        return;
    }

    if (!last_mod_time_.has_value() || *current_mod_time != *last_mod_time_) {
        RENDU_LOG_INFO("Config file modified, reloading: {}", file_path_);
        auto result = reload();
        if (std::holds_alternative<Error>(result)) {
            RENDU_LOG_ERROR("Failed to reload config: {}", std::get<Error>(result).message());
        }
    }
}

std::optional<std::filesystem::file_time_type> ConfigWatcher::get_file_modification_time() const {
    try {
        std::error_code ec;
        auto mod_time = std::filesystem::last_write_time(file_path_, ec);
        if (ec) {
            RENDU_LOG_ERROR("Failed to get file modification time: {}", ec.message());
            return std::nullopt;
        }
        return mod_time;
    } catch (const std::exception& e) {
        RENDU_LOG_ERROR("Exception getting file modification time: {}", e.what());
        return std::nullopt;
    }
}

bool ConfigWatcher::file_exists() const {
    try {
        std::error_code ec;
        return std::filesystem::exists(file_path_, ec);
    } catch (const std::exception& e) {
        RENDU_LOG_ERROR("Exception checking file existence: {}", e.what());
        return false;
    }
}

} // namespace config
END_NAMESPACE_COMMON
