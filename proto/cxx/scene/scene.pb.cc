// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: scene/scene.proto

#include "scene/scene.pb.h"

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
namespace scene {
namespace model {
PROTOBUF_CONSTEXPR scene_info::scene_info(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.name_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.id_)*/0
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct scene_infoDefaultTypeInternal {
  PROTOBUF_CONSTEXPR scene_infoDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~scene_infoDefaultTypeInternal() {}
  union {
    scene_info _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 scene_infoDefaultTypeInternal _scene_info_default_instance_;
}  // namespace model
}  // namespace scene
}  // namespace rendu
static ::_pb::Metadata file_level_metadata_scene_2fscene_2eproto[1];
static constexpr ::_pb::EnumDescriptor const** file_level_enum_descriptors_scene_2fscene_2eproto = nullptr;
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_scene_2fscene_2eproto = nullptr;

const uint32_t TableStruct_scene_2fscene_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::rendu::scene::model::scene_info, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::rendu::scene::model::scene_info, _impl_.name_),
  PROTOBUF_FIELD_OFFSET(::rendu::scene::model::scene_info, _impl_.id_),
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::rendu::scene::model::scene_info)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::rendu::scene::model::_scene_info_default_instance_._instance,
};

const char descriptor_table_protodef_scene_2fscene_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\021scene/scene.proto\022\021rendu.scene.model\"&"
  "\n\nscene_info\022\014\n\004name\030\001 \001(\t\022\n\n\002id\030\002 \001(\005b\006"
  "proto3"
  ;
static ::_pbi::once_flag descriptor_table_scene_2fscene_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_scene_2fscene_2eproto = {
    false, false, 86, descriptor_table_protodef_scene_2fscene_2eproto,
    "scene/scene.proto",
    &descriptor_table_scene_2fscene_2eproto_once, nullptr, 0, 1,
    schemas, file_default_instances, TableStruct_scene_2fscene_2eproto::offsets,
    file_level_metadata_scene_2fscene_2eproto, file_level_enum_descriptors_scene_2fscene_2eproto,
    file_level_service_descriptors_scene_2fscene_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_scene_2fscene_2eproto_getter() {
  return &descriptor_table_scene_2fscene_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_scene_2fscene_2eproto(&descriptor_table_scene_2fscene_2eproto);
namespace rendu {
namespace scene {
namespace model {

// ===================================================================

class scene_info::_Internal {
 public:
};

scene_info::scene_info(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:rendu.scene.model.scene_info)
}
scene_info::scene_info(const scene_info& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  scene_info* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.name_){}
    , decltype(_impl_.id_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _impl_.name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_name().empty()) {
    _this->_impl_.name_.Set(from._internal_name(), 
      _this->GetArenaForAllocation());
  }
  _this->_impl_.id_ = from._impl_.id_;
  // @@protoc_insertion_point(copy_constructor:rendu.scene.model.scene_info)
}

inline void scene_info::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.name_){}
    , decltype(_impl_.id_){0}
    , /*decltype(_impl_._cached_size_)*/{}
  };
  _impl_.name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
}

scene_info::~scene_info() {
  // @@protoc_insertion_point(destructor:rendu.scene.model.scene_info)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void scene_info::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.name_.Destroy();
}

void scene_info::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void scene_info::Clear() {
// @@protoc_insertion_point(message_clear_start:rendu.scene.model.scene_info)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.name_.ClearToEmpty();
  _impl_.id_ = 0;
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* scene_info::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // string name = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 10)) {
          auto str = _internal_mutable_name();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "rendu.scene.model.scene_info.name"));
        } else
          goto handle_unusual;
        continue;
      // int32 id = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          _impl_.id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
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

uint8_t* scene_info::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:rendu.scene.model.scene_info)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // string name = 1;
  if (!this->_internal_name().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_name().data(), static_cast<int>(this->_internal_name().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "rendu.scene.model.scene_info.name");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_name(), target);
  }

  // int32 id = 2;
  if (this->_internal_id() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteInt32ToArray(2, this->_internal_id(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:rendu.scene.model.scene_info)
  return target;
}

size_t scene_info::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:rendu.scene.model.scene_info)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string name = 1;
  if (!this->_internal_name().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_name());
  }

  // int32 id = 2;
  if (this->_internal_id() != 0) {
    total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(this->_internal_id());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData scene_info::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    scene_info::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*scene_info::GetClassData() const { return &_class_data_; }


void scene_info::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<scene_info*>(&to_msg);
  auto& from = static_cast<const scene_info&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:rendu.scene.model.scene_info)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_name().empty()) {
    _this->_internal_set_name(from._internal_name());
  }
  if (from._internal_id() != 0) {
    _this->_internal_set_id(from._internal_id());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void scene_info::CopyFrom(const scene_info& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:rendu.scene.model.scene_info)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool scene_info::IsInitialized() const {
  return true;
}

void scene_info::InternalSwap(scene_info* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.name_, lhs_arena,
      &other->_impl_.name_, rhs_arena
  );
  swap(_impl_.id_, other->_impl_.id_);
}

::PROTOBUF_NAMESPACE_ID::Metadata scene_info::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_scene_2fscene_2eproto_getter, &descriptor_table_scene_2fscene_2eproto_once,
      file_level_metadata_scene_2fscene_2eproto[0]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace model
}  // namespace scene
}  // namespace rendu
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::rendu::scene::model::scene_info*
Arena::CreateMaybeMessage< ::rendu::scene::model::scene_info >(Arena* arena) {
  return Arena::CreateMessageInternal< ::rendu::scene::model::scene_info >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
