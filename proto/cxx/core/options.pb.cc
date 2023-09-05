// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: core/options.proto

#include "core/options.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

namespace rendu {
namespace core {
namespace options {
PROTOBUF_CONSTEXPR Options::Options(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.envtype_)*/0
  , /*decltype(_impl_.hosttype_)*/0
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct OptionsDefaultTypeInternal {
  PROTOBUF_CONSTEXPR OptionsDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~OptionsDefaultTypeInternal() {}
  union {
    Options _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 OptionsDefaultTypeInternal _Options_default_instance_;
}  // namespace options
}  // namespace core
}  // namespace rendu
static ::_pb::Metadata file_level_metadata_core_2foptions_2eproto[1];
static const ::_pb::EnumDescriptor* file_level_enum_descriptors_core_2foptions_2eproto[2];
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_core_2foptions_2eproto = nullptr;

const uint32_t TableStruct_core_2foptions_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::rendu::core::options::Options, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::rendu::core::options::Options, _impl_.envtype_),
  PROTOBUF_FIELD_OFFSET(::rendu::core::options::Options, _impl_.hosttype_),
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::rendu::core::options::Options)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::rendu::core::options::_Options_default_instance_._instance,
};

const char descriptor_table_protodef_core_2foptions_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\022core/options.proto\022\022rendu.core.options"
  "\"g\n\007Options\022,\n\007envType\030\001 \001(\0162\033.rendu.cor"
  "e.options.EnvType\022.\n\010hostType\030\002 \001(\0162\034.re"
  "ndu.core.options.HostType*%\n\007EnvType\022\013\n\007"
  "ETDebug\020\000\022\r\n\tETRelease\020\001*H\n\010HostType\022\007\n\003"
  "ALL\020\000\022\010\n\004Zone\020\001\022\010\n\004Gate\020\002\022\t\n\005Realm\020\003\022\t\n\005"
  "World\020\004\022\t\n\005Scene\020\005b\006proto3"
  ;
static ::_pbi::once_flag descriptor_table_core_2foptions_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_core_2foptions_2eproto = {
    false, false, 266, descriptor_table_protodef_core_2foptions_2eproto,
    "core/options.proto",
    &descriptor_table_core_2foptions_2eproto_once, nullptr, 0, 1,
    schemas, file_default_instances, TableStruct_core_2foptions_2eproto::offsets,
    file_level_metadata_core_2foptions_2eproto, file_level_enum_descriptors_core_2foptions_2eproto,
    file_level_service_descriptors_core_2foptions_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_core_2foptions_2eproto_getter() {
  return &descriptor_table_core_2foptions_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_core_2foptions_2eproto(&descriptor_table_core_2foptions_2eproto);
namespace rendu {
namespace core {
namespace options {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* EnvType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_core_2foptions_2eproto);
  return file_level_enum_descriptors_core_2foptions_2eproto[0];
}
bool EnvType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* HostType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_core_2foptions_2eproto);
  return file_level_enum_descriptors_core_2foptions_2eproto[1];
}
bool HostType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      return true;
    default:
      return false;
  }
}


// ===================================================================

class Options::_Internal {
 public:
};

Options::Options(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:rendu.core.options.Options)
}
Options::Options(const Options& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  Options* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.envtype_){}
    , decltype(_impl_.hosttype_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::memcpy(&_impl_.envtype_, &from._impl_.envtype_,
    static_cast<size_t>(reinterpret_cast<char*>(&_impl_.hosttype_) -
    reinterpret_cast<char*>(&_impl_.envtype_)) + sizeof(_impl_.hosttype_));
  // @@protoc_insertion_point(copy_constructor:rendu.core.options.Options)
}

inline void Options::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.envtype_){0}
    , decltype(_impl_.hosttype_){0}
    , /*decltype(_impl_._cached_size_)*/{}
  };
}

Options::~Options() {
  // @@protoc_insertion_point(destructor:rendu.core.options.Options)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void Options::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
}

void Options::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void Options::Clear() {
// @@protoc_insertion_point(message_clear_start:rendu.core.options.Options)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&_impl_.envtype_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&_impl_.hosttype_) -
      reinterpret_cast<char*>(&_impl_.envtype_)) + sizeof(_impl_.hosttype_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* Options::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // .rendu.core.options.EnvType envType = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          uint64_t val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_envtype(static_cast<::rendu::core::options::EnvType>(val));
        } else
          goto handle_unusual;
        continue;
      // .rendu.core.options.HostType hostType = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          uint64_t val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_hosttype(static_cast<::rendu::core::options::HostType>(val));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* Options::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:rendu.core.options.Options)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // .rendu.core.options.EnvType envType = 1;
  if (this->_internal_envtype() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteEnumToArray(
      1, this->_internal_envtype(), target);
  }

  // .rendu.core.options.HostType hostType = 2;
  if (this->_internal_hosttype() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteEnumToArray(
      2, this->_internal_hosttype(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:rendu.core.options.Options)
  return target;
}

size_t Options::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:rendu.core.options.Options)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // .rendu.core.options.EnvType envType = 1;
  if (this->_internal_envtype() != 0) {
    total_size += 1 +
      ::_pbi::WireFormatLite::EnumSize(this->_internal_envtype());
  }

  // .rendu.core.options.HostType hostType = 2;
  if (this->_internal_hosttype() != 0) {
    total_size += 1 +
      ::_pbi::WireFormatLite::EnumSize(this->_internal_hosttype());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData Options::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    Options::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*Options::GetClassData() const { return &_class_data_; }


void Options::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<Options*>(&to_msg);
  auto& from = static_cast<const Options&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:rendu.core.options.Options)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_envtype() != 0) {
    _this->_internal_set_envtype(from._internal_envtype());
  }
  if (from._internal_hosttype() != 0) {
    _this->_internal_set_hosttype(from._internal_hosttype());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void Options::CopyFrom(const Options& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:rendu.core.options.Options)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Options::IsInitialized() const {
  return true;
}

void Options::InternalSwap(Options* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(Options, _impl_.hosttype_)
      + sizeof(Options::_impl_.hosttype_)
      - PROTOBUF_FIELD_OFFSET(Options, _impl_.envtype_)>(
          reinterpret_cast<char*>(&_impl_.envtype_),
          reinterpret_cast<char*>(&other->_impl_.envtype_));
}

::PROTOBUF_NAMESPACE_ID::Metadata Options::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_core_2foptions_2eproto_getter, &descriptor_table_core_2foptions_2eproto_once,
      file_level_metadata_core_2foptions_2eproto[0]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace options
}  // namespace core
}  // namespace rendu
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::rendu::core::options::Options*
Arena::CreateMaybeMessage< ::rendu::core::options::Options >(Arena* arena) {
  return Arena::CreateMessageInternal< ::rendu::core::options::Options >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
