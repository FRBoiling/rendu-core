// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: core/options.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_core_2foptions_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_core_2foptions_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_core_2foptions_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_core_2foptions_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_core_2foptions_2eproto;
namespace rendu {
namespace proto {
namespace core {
class Options;
struct OptionsDefaultTypeInternal;
extern OptionsDefaultTypeInternal _Options_default_instance_;
}  // namespace core
}  // namespace proto
}  // namespace rendu
PROTOBUF_NAMESPACE_OPEN
template<> ::rendu::proto::core::Options* Arena::CreateMaybeMessage<::rendu::proto::core::Options>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace rendu {
namespace proto {
namespace core {

enum DevelopModeType : int {
  Official = 0,
  Develop = 1,
  Pressure = 2,
  DevelopModeType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  DevelopModeType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool DevelopModeType_IsValid(int value);
constexpr DevelopModeType DevelopModeType_MIN = Official;
constexpr DevelopModeType DevelopModeType_MAX = Pressure;
constexpr int DevelopModeType_ARRAYSIZE = DevelopModeType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* DevelopModeType_descriptor();
template<typename T>
inline const std::string& DevelopModeType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, DevelopModeType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function DevelopModeType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    DevelopModeType_descriptor(), enum_t_value);
}
inline bool DevelopModeType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, DevelopModeType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<DevelopModeType>(
    DevelopModeType_descriptor(), name, value);
}
enum AppType : int {
  Server = 0,
  Watcher = 1,
  GameTool = 2,
  ExcelExporter = 3,
  Proto2CS = 4,
  BenchmarkClient = 5,
  BenchmarkServer = 6,
  Demo = 10,
  LockStep = 11,
  AppType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  AppType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool AppType_IsValid(int value);
constexpr AppType AppType_MIN = Server;
constexpr AppType AppType_MAX = LockStep;
constexpr int AppType_ARRAYSIZE = AppType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* AppType_descriptor();
template<typename T>
inline const std::string& AppType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, AppType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function AppType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    AppType_descriptor(), enum_t_value);
}
inline bool AppType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, AppType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<AppType>(
    AppType_descriptor(), name, value);
}
// ===================================================================

class Options final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:rendu.proto.core.Options) */ {
 public:
  inline Options() : Options(nullptr) {}
  ~Options() override;
  explicit PROTOBUF_CONSTEXPR Options(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Options(const Options& from);
  Options(Options&& from) noexcept
    : Options() {
    *this = ::std::move(from);
  }

  inline Options& operator=(const Options& from) {
    CopyFrom(from);
    return *this;
  }
  inline Options& operator=(Options&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const Options& default_instance() {
    return *internal_default_instance();
  }
  static inline const Options* internal_default_instance() {
    return reinterpret_cast<const Options*>(
               &_Options_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Options& a, Options& b) {
    a.Swap(&b);
  }
  inline void Swap(Options* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Options* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Options* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Options>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const Options& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const Options& from) {
    Options::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Options* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "rendu.proto.core.Options";
  }
  protected:
  explicit Options(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kStartConfigFieldNumber = 2,
    kAppTypeFieldNumber = 1,
    kProcessFieldNumber = 3,
    kDevelopFieldNumber = 4,
    kLogLevelFieldNumber = 5,
    kConsoleFieldNumber = 6,
  };
  // string startConfig = 2;
  void clear_startconfig();
  const std::string& startconfig() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_startconfig(ArgT0&& arg0, ArgT... args);
  std::string* mutable_startconfig();
  PROTOBUF_NODISCARD std::string* release_startconfig();
  void set_allocated_startconfig(std::string* startconfig);
  private:
  const std::string& _internal_startconfig() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_startconfig(const std::string& value);
  std::string* _internal_mutable_startconfig();
  public:

  // .rendu.proto.core.AppType appType = 1;
  void clear_apptype();
  ::rendu::proto::core::AppType apptype() const;
  void set_apptype(::rendu::proto::core::AppType value);
  private:
  ::rendu::proto::core::AppType _internal_apptype() const;
  void _internal_set_apptype(::rendu::proto::core::AppType value);
  public:

  // int32 process = 3;
  void clear_process();
  int32_t process() const;
  void set_process(int32_t value);
  private:
  int32_t _internal_process() const;
  void _internal_set_process(int32_t value);
  public:

  // .rendu.proto.core.DevelopModeType develop = 4;
  void clear_develop();
  ::rendu::proto::core::DevelopModeType develop() const;
  void set_develop(::rendu::proto::core::DevelopModeType value);
  private:
  ::rendu::proto::core::DevelopModeType _internal_develop() const;
  void _internal_set_develop(::rendu::proto::core::DevelopModeType value);
  public:

  // int32 logLevel = 5;
  void clear_loglevel();
  int32_t loglevel() const;
  void set_loglevel(int32_t value);
  private:
  int32_t _internal_loglevel() const;
  void _internal_set_loglevel(int32_t value);
  public:

  // int32 console = 6;
  void clear_console();
  int32_t console() const;
  void set_console(int32_t value);
  private:
  int32_t _internal_console() const;
  void _internal_set_console(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:rendu.proto.core.Options)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr startconfig_;
    int apptype_;
    int32_t process_;
    int develop_;
    int32_t loglevel_;
    int32_t console_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_core_2foptions_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Options

// .rendu.proto.core.AppType appType = 1;
inline void Options::clear_apptype() {
  _impl_.apptype_ = 0;
}
inline ::rendu::proto::core::AppType Options::_internal_apptype() const {
  return static_cast< ::rendu::proto::core::AppType >(_impl_.apptype_);
}
inline ::rendu::proto::core::AppType Options::apptype() const {
  // @@protoc_insertion_point(field_get:rendu.proto.core.Options.appType)
  return _internal_apptype();
}
inline void Options::_internal_set_apptype(::rendu::proto::core::AppType value) {
  
  _impl_.apptype_ = value;
}
inline void Options::set_apptype(::rendu::proto::core::AppType value) {
  _internal_set_apptype(value);
  // @@protoc_insertion_point(field_set:rendu.proto.core.Options.appType)
}

// string startConfig = 2;
inline void Options::clear_startconfig() {
  _impl_.startconfig_.ClearToEmpty();
}
inline const std::string& Options::startconfig() const {
  // @@protoc_insertion_point(field_get:rendu.proto.core.Options.startConfig)
  return _internal_startconfig();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Options::set_startconfig(ArgT0&& arg0, ArgT... args) {
 
 _impl_.startconfig_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:rendu.proto.core.Options.startConfig)
}
inline std::string* Options::mutable_startconfig() {
  std::string* _s = _internal_mutable_startconfig();
  // @@protoc_insertion_point(field_mutable:rendu.proto.core.Options.startConfig)
  return _s;
}
inline const std::string& Options::_internal_startconfig() const {
  return _impl_.startconfig_.Get();
}
inline void Options::_internal_set_startconfig(const std::string& value) {
  
  _impl_.startconfig_.Set(value, GetArenaForAllocation());
}
inline std::string* Options::_internal_mutable_startconfig() {
  
  return _impl_.startconfig_.Mutable(GetArenaForAllocation());
}
inline std::string* Options::release_startconfig() {
  // @@protoc_insertion_point(field_release:rendu.proto.core.Options.startConfig)
  return _impl_.startconfig_.Release();
}
inline void Options::set_allocated_startconfig(std::string* startconfig) {
  if (startconfig != nullptr) {
    
  } else {
    
  }
  _impl_.startconfig_.SetAllocated(startconfig, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.startconfig_.IsDefault()) {
    _impl_.startconfig_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:rendu.proto.core.Options.startConfig)
}

// int32 process = 3;
inline void Options::clear_process() {
  _impl_.process_ = 0;
}
inline int32_t Options::_internal_process() const {
  return _impl_.process_;
}
inline int32_t Options::process() const {
  // @@protoc_insertion_point(field_get:rendu.proto.core.Options.process)
  return _internal_process();
}
inline void Options::_internal_set_process(int32_t value) {
  
  _impl_.process_ = value;
}
inline void Options::set_process(int32_t value) {
  _internal_set_process(value);
  // @@protoc_insertion_point(field_set:rendu.proto.core.Options.process)
}

// .rendu.proto.core.DevelopModeType develop = 4;
inline void Options::clear_develop() {
  _impl_.develop_ = 0;
}
inline ::rendu::proto::core::DevelopModeType Options::_internal_develop() const {
  return static_cast< ::rendu::proto::core::DevelopModeType >(_impl_.develop_);
}
inline ::rendu::proto::core::DevelopModeType Options::develop() const {
  // @@protoc_insertion_point(field_get:rendu.proto.core.Options.develop)
  return _internal_develop();
}
inline void Options::_internal_set_develop(::rendu::proto::core::DevelopModeType value) {
  
  _impl_.develop_ = value;
}
inline void Options::set_develop(::rendu::proto::core::DevelopModeType value) {
  _internal_set_develop(value);
  // @@protoc_insertion_point(field_set:rendu.proto.core.Options.develop)
}

// int32 logLevel = 5;
inline void Options::clear_loglevel() {
  _impl_.loglevel_ = 0;
}
inline int32_t Options::_internal_loglevel() const {
  return _impl_.loglevel_;
}
inline int32_t Options::loglevel() const {
  // @@protoc_insertion_point(field_get:rendu.proto.core.Options.logLevel)
  return _internal_loglevel();
}
inline void Options::_internal_set_loglevel(int32_t value) {
  
  _impl_.loglevel_ = value;
}
inline void Options::set_loglevel(int32_t value) {
  _internal_set_loglevel(value);
  // @@protoc_insertion_point(field_set:rendu.proto.core.Options.logLevel)
}

// int32 console = 6;
inline void Options::clear_console() {
  _impl_.console_ = 0;
}
inline int32_t Options::_internal_console() const {
  return _impl_.console_;
}
inline int32_t Options::console() const {
  // @@protoc_insertion_point(field_get:rendu.proto.core.Options.console)
  return _internal_console();
}
inline void Options::_internal_set_console(int32_t value) {
  
  _impl_.console_ = value;
}
inline void Options::set_console(int32_t value) {
  _internal_set_console(value);
  // @@protoc_insertion_point(field_set:rendu.proto.core.Options.console)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace core
}  // namespace proto
}  // namespace rendu

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::rendu::proto::core::DevelopModeType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::rendu::proto::core::DevelopModeType>() {
  return ::rendu::proto::core::DevelopModeType_descriptor();
}
template <> struct is_proto_enum< ::rendu::proto::core::AppType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::rendu::proto::core::AppType>() {
  return ::rendu::proto::core::AppType_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_core_2foptions_2eproto
