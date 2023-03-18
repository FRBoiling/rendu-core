// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: addressbook/addressbook.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_addressbook_2faddressbook_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_addressbook_2faddressbook_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3020000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3020003 < PROTOBUF_MIN_PROTOC_VERSION
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
#include <google/protobuf/timestamp.pb.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_addressbook_2faddressbook_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_addressbook_2faddressbook_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_addressbook_2faddressbook_2eproto;
namespace game {
namespace address_book {
class AddressBook;
struct AddressBookDefaultTypeInternal;
extern AddressBookDefaultTypeInternal _AddressBook_default_instance_;
class Person;
struct PersonDefaultTypeInternal;
extern PersonDefaultTypeInternal _Person_default_instance_;
class Person_PhoneNumber;
struct Person_PhoneNumberDefaultTypeInternal;
extern Person_PhoneNumberDefaultTypeInternal _Person_PhoneNumber_default_instance_;
}  // namespace address_book
}  // namespace game
PROTOBUF_NAMESPACE_OPEN
template<> ::game::address_book::AddressBook* Arena::CreateMaybeMessage<::game::address_book::AddressBook>(Arena*);
template<> ::game::address_book::Person* Arena::CreateMaybeMessage<::game::address_book::Person>(Arena*);
template<> ::game::address_book::Person_PhoneNumber* Arena::CreateMaybeMessage<::game::address_book::Person_PhoneNumber>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace game {
namespace address_book {

enum Person_PhoneType : int {
  Person_PhoneType_MOBILE = 0,
  Person_PhoneType_HOME = 1,
  Person_PhoneType_WORK = 2,
  Person_PhoneType_Person_PhoneType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  Person_PhoneType_Person_PhoneType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool Person_PhoneType_IsValid(int value);
constexpr Person_PhoneType Person_PhoneType_PhoneType_MIN = Person_PhoneType_MOBILE;
constexpr Person_PhoneType Person_PhoneType_PhoneType_MAX = Person_PhoneType_WORK;
constexpr int Person_PhoneType_PhoneType_ARRAYSIZE = Person_PhoneType_PhoneType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* Person_PhoneType_descriptor();
template<typename T>
inline const std::string& Person_PhoneType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, Person_PhoneType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function Person_PhoneType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    Person_PhoneType_descriptor(), enum_t_value);
}
inline bool Person_PhoneType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, Person_PhoneType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<Person_PhoneType>(
    Person_PhoneType_descriptor(), name, value);
}
// ===================================================================

class Person_PhoneNumber final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:game.address_book.Person.PhoneNumber) */ {
 public:
  inline Person_PhoneNumber() : Person_PhoneNumber(nullptr) {}
  ~Person_PhoneNumber() override;
  explicit PROTOBUF_CONSTEXPR Person_PhoneNumber(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Person_PhoneNumber(const Person_PhoneNumber& from);
  Person_PhoneNumber(Person_PhoneNumber&& from) noexcept
    : Person_PhoneNumber() {
    *this = ::std::move(from);
  }

  inline Person_PhoneNumber& operator=(const Person_PhoneNumber& from) {
    CopyFrom(from);
    return *this;
  }
  inline Person_PhoneNumber& operator=(Person_PhoneNumber&& from) noexcept {
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
  static const Person_PhoneNumber& default_instance() {
    return *internal_default_instance();
  }
  static inline const Person_PhoneNumber* internal_default_instance() {
    return reinterpret_cast<const Person_PhoneNumber*>(
               &_Person_PhoneNumber_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Person_PhoneNumber& a, Person_PhoneNumber& b) {
    a.Swap(&b);
  }
  inline void Swap(Person_PhoneNumber* other) {
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
  void UnsafeArenaSwap(Person_PhoneNumber* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Person_PhoneNumber* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Person_PhoneNumber>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const Person_PhoneNumber& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const Person_PhoneNumber& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Person_PhoneNumber* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "game.address_book.Person.PhoneNumber";
  }
  protected:
  explicit Person_PhoneNumber(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kNumberFieldNumber = 1,
    kTypeFieldNumber = 2,
  };
  // string number = 1;
  void clear_number();
  const std::string& number() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_number(ArgT0&& arg0, ArgT... args);
  std::string* mutable_number();
  PROTOBUF_NODISCARD std::string* release_number();
  void set_allocated_number(std::string* number);
  private:
  const std::string& _internal_number() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_number(const std::string& value);
  std::string* _internal_mutable_number();
  public:

  // .game.address_book.Person.PhoneType type = 2;
  void clear_type();
  ::game::address_book::Person_PhoneType type() const;
  void set_type(::game::address_book::Person_PhoneType value);
  private:
  ::game::address_book::Person_PhoneType _internal_type() const;
  void _internal_set_type(::game::address_book::Person_PhoneType value);
  public:

  // @@protoc_insertion_point(class_scope:game.address_book.Person.PhoneNumber)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr number_;
  int type_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_addressbook_2faddressbook_2eproto;
};
// -------------------------------------------------------------------

class Person final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:game.address_book.Person) */ {
 public:
  inline Person() : Person(nullptr) {}
  ~Person() override;
  explicit PROTOBUF_CONSTEXPR Person(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Person(const Person& from);
  Person(Person&& from) noexcept
    : Person() {
    *this = ::std::move(from);
  }

  inline Person& operator=(const Person& from) {
    CopyFrom(from);
    return *this;
  }
  inline Person& operator=(Person&& from) noexcept {
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
  static const Person& default_instance() {
    return *internal_default_instance();
  }
  static inline const Person* internal_default_instance() {
    return reinterpret_cast<const Person*>(
               &_Person_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(Person& a, Person& b) {
    a.Swap(&b);
  }
  inline void Swap(Person* other) {
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
  void UnsafeArenaSwap(Person* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Person* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Person>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const Person& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const Person& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Person* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "game.address_book.Person";
  }
  protected:
  explicit Person(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  typedef Person_PhoneNumber PhoneNumber;

  typedef Person_PhoneType PhoneType;
  static constexpr PhoneType MOBILE =
    Person_PhoneType_MOBILE;
  static constexpr PhoneType HOME =
    Person_PhoneType_HOME;
  static constexpr PhoneType WORK =
    Person_PhoneType_WORK;
  static inline bool PhoneType_IsValid(int value) {
    return Person_PhoneType_IsValid(value);
  }
  static constexpr PhoneType PhoneType_MIN =
    Person_PhoneType_PhoneType_MIN;
  static constexpr PhoneType PhoneType_MAX =
    Person_PhoneType_PhoneType_MAX;
  static constexpr int PhoneType_ARRAYSIZE =
    Person_PhoneType_PhoneType_ARRAYSIZE;
  static inline const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor*
  PhoneType_descriptor() {
    return Person_PhoneType_descriptor();
  }
  template<typename T>
  static inline const std::string& PhoneType_Name(T enum_t_value) {
    static_assert(::std::is_same<T, PhoneType>::value ||
      ::std::is_integral<T>::value,
      "Incorrect type passed to function PhoneType_Name.");
    return Person_PhoneType_Name(enum_t_value);
  }
  static inline bool PhoneType_Parse(::PROTOBUF_NAMESPACE_ID::ConstStringParam name,
      PhoneType* value) {
    return Person_PhoneType_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  enum : int {
    kPhonesFieldNumber = 4,
    kNameFieldNumber = 1,
    kEmailFieldNumber = 3,
    kLastUpdatedFieldNumber = 5,
    kIdFieldNumber = 2,
  };
  // repeated .game.address_book.Person.PhoneNumber phones = 4;
  int phones_size() const;
  private:
  int _internal_phones_size() const;
  public:
  void clear_phones();
  ::game::address_book::Person_PhoneNumber* mutable_phones(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::game::address_book::Person_PhoneNumber >*
      mutable_phones();
  private:
  const ::game::address_book::Person_PhoneNumber& _internal_phones(int index) const;
  ::game::address_book::Person_PhoneNumber* _internal_add_phones();
  public:
  const ::game::address_book::Person_PhoneNumber& phones(int index) const;
  ::game::address_book::Person_PhoneNumber* add_phones();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::game::address_book::Person_PhoneNumber >&
      phones() const;

  // string name = 1;
  void clear_name();
  const std::string& name() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_name(ArgT0&& arg0, ArgT... args);
  std::string* mutable_name();
  PROTOBUF_NODISCARD std::string* release_name();
  void set_allocated_name(std::string* name);
  private:
  const std::string& _internal_name() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_name(const std::string& value);
  std::string* _internal_mutable_name();
  public:

  // string email = 3;
  void clear_email();
  const std::string& email() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_email(ArgT0&& arg0, ArgT... args);
  std::string* mutable_email();
  PROTOBUF_NODISCARD std::string* release_email();
  void set_allocated_email(std::string* email);
  private:
  const std::string& _internal_email() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_email(const std::string& value);
  std::string* _internal_mutable_email();
  public:

  // .google.protobuf.Timestamp last_updated = 5;
  bool has_last_updated() const;
  private:
  bool _internal_has_last_updated() const;
  public:
  void clear_last_updated();
  const ::PROTOBUF_NAMESPACE_ID::Timestamp& last_updated() const;
  PROTOBUF_NODISCARD ::PROTOBUF_NAMESPACE_ID::Timestamp* release_last_updated();
  ::PROTOBUF_NAMESPACE_ID::Timestamp* mutable_last_updated();
  void set_allocated_last_updated(::PROTOBUF_NAMESPACE_ID::Timestamp* last_updated);
  private:
  const ::PROTOBUF_NAMESPACE_ID::Timestamp& _internal_last_updated() const;
  ::PROTOBUF_NAMESPACE_ID::Timestamp* _internal_mutable_last_updated();
  public:
  void unsafe_arena_set_allocated_last_updated(
      ::PROTOBUF_NAMESPACE_ID::Timestamp* last_updated);
  ::PROTOBUF_NAMESPACE_ID::Timestamp* unsafe_arena_release_last_updated();

  // int32 id = 2;
  void clear_id();
  int32_t id() const;
  void set_id(int32_t value);
  private:
  int32_t _internal_id() const;
  void _internal_set_id(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:game.address_book.Person)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::game::address_book::Person_PhoneNumber > phones_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr name_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr email_;
  ::PROTOBUF_NAMESPACE_ID::Timestamp* last_updated_;
  int32_t id_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_addressbook_2faddressbook_2eproto;
};
// -------------------------------------------------------------------

class AddressBook final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:game.address_book.AddressBook) */ {
 public:
  inline AddressBook() : AddressBook(nullptr) {}
  ~AddressBook() override;
  explicit PROTOBUF_CONSTEXPR AddressBook(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  AddressBook(const AddressBook& from);
  AddressBook(AddressBook&& from) noexcept
    : AddressBook() {
    *this = ::std::move(from);
  }

  inline AddressBook& operator=(const AddressBook& from) {
    CopyFrom(from);
    return *this;
  }
  inline AddressBook& operator=(AddressBook&& from) noexcept {
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
  static const AddressBook& default_instance() {
    return *internal_default_instance();
  }
  static inline const AddressBook* internal_default_instance() {
    return reinterpret_cast<const AddressBook*>(
               &_AddressBook_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  friend void swap(AddressBook& a, AddressBook& b) {
    a.Swap(&b);
  }
  inline void Swap(AddressBook* other) {
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
  void UnsafeArenaSwap(AddressBook* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  AddressBook* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<AddressBook>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const AddressBook& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const AddressBook& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(AddressBook* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "game.address_book.AddressBook";
  }
  protected:
  explicit AddressBook(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kPeopleFieldNumber = 1,
  };
  // repeated .game.address_book.Person people = 1;
  int people_size() const;
  private:
  int _internal_people_size() const;
  public:
  void clear_people();
  ::game::address_book::Person* mutable_people(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::game::address_book::Person >*
      mutable_people();
  private:
  const ::game::address_book::Person& _internal_people(int index) const;
  ::game::address_book::Person* _internal_add_people();
  public:
  const ::game::address_book::Person& people(int index) const;
  ::game::address_book::Person* add_people();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::game::address_book::Person >&
      people() const;

  // @@protoc_insertion_point(class_scope:game.address_book.AddressBook)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::game::address_book::Person > people_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_addressbook_2faddressbook_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Person_PhoneNumber

// string number = 1;
inline void Person_PhoneNumber::clear_number() {
  number_.ClearToEmpty();
}
inline const std::string& Person_PhoneNumber::number() const {
  // @@protoc_insertion_point(field_get:game.address_book.Person.PhoneNumber.number)
  return _internal_number();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Person_PhoneNumber::set_number(ArgT0&& arg0, ArgT... args) {
 
 number_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:game.address_book.Person.PhoneNumber.number)
}
inline std::string* Person_PhoneNumber::mutable_number() {
  std::string* _s = _internal_mutable_number();
  // @@protoc_insertion_point(field_mutable:game.address_book.Person.PhoneNumber.number)
  return _s;
}
inline const std::string& Person_PhoneNumber::_internal_number() const {
  return number_.Get();
}
inline void Person_PhoneNumber::_internal_set_number(const std::string& value) {
  
  number_.Set(value, GetArenaForAllocation());
}
inline std::string* Person_PhoneNumber::_internal_mutable_number() {
  
  return number_.Mutable(GetArenaForAllocation());
}
inline std::string* Person_PhoneNumber::release_number() {
  // @@protoc_insertion_point(field_release:game.address_book.Person.PhoneNumber.number)
  return number_.Release();
}
inline void Person_PhoneNumber::set_allocated_number(std::string* number) {
  if (number != nullptr) {
    
  } else {
    
  }
  number_.SetAllocated(number, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (number_.IsDefault()) {
    number_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:game.address_book.Person.PhoneNumber.number)
}

// .game.address_book.Person.PhoneType type = 2;
inline void Person_PhoneNumber::clear_type() {
  type_ = 0;
}
inline ::game::address_book::Person_PhoneType Person_PhoneNumber::_internal_type() const {
  return static_cast< ::game::address_book::Person_PhoneType >(type_);
}
inline ::game::address_book::Person_PhoneType Person_PhoneNumber::type() const {
  // @@protoc_insertion_point(field_get:game.address_book.Person.PhoneNumber.type)
  return _internal_type();
}
inline void Person_PhoneNumber::_internal_set_type(::game::address_book::Person_PhoneType value) {
  
  type_ = value;
}
inline void Person_PhoneNumber::set_type(::game::address_book::Person_PhoneType value) {
  _internal_set_type(value);
  // @@protoc_insertion_point(field_set:game.address_book.Person.PhoneNumber.type)
}

// -------------------------------------------------------------------

// Person

// string name = 1;
inline void Person::clear_name() {
  name_.ClearToEmpty();
}
inline const std::string& Person::name() const {
  // @@protoc_insertion_point(field_get:game.address_book.Person.name)
  return _internal_name();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Person::set_name(ArgT0&& arg0, ArgT... args) {
 
 name_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:game.address_book.Person.name)
}
inline std::string* Person::mutable_name() {
  std::string* _s = _internal_mutable_name();
  // @@protoc_insertion_point(field_mutable:game.address_book.Person.name)
  return _s;
}
inline const std::string& Person::_internal_name() const {
  return name_.Get();
}
inline void Person::_internal_set_name(const std::string& value) {
  
  name_.Set(value, GetArenaForAllocation());
}
inline std::string* Person::_internal_mutable_name() {
  
  return name_.Mutable(GetArenaForAllocation());
}
inline std::string* Person::release_name() {
  // @@protoc_insertion_point(field_release:game.address_book.Person.name)
  return name_.Release();
}
inline void Person::set_allocated_name(std::string* name) {
  if (name != nullptr) {
    
  } else {
    
  }
  name_.SetAllocated(name, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (name_.IsDefault()) {
    name_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:game.address_book.Person.name)
}

// int32 id = 2;
inline void Person::clear_id() {
  id_ = 0;
}
inline int32_t Person::_internal_id() const {
  return id_;
}
inline int32_t Person::id() const {
  // @@protoc_insertion_point(field_get:game.address_book.Person.id)
  return _internal_id();
}
inline void Person::_internal_set_id(int32_t value) {
  
  id_ = value;
}
inline void Person::set_id(int32_t value) {
  _internal_set_id(value);
  // @@protoc_insertion_point(field_set:game.address_book.Person.id)
}

// string email = 3;
inline void Person::clear_email() {
  email_.ClearToEmpty();
}
inline const std::string& Person::email() const {
  // @@protoc_insertion_point(field_get:game.address_book.Person.email)
  return _internal_email();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Person::set_email(ArgT0&& arg0, ArgT... args) {
 
 email_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:game.address_book.Person.email)
}
inline std::string* Person::mutable_email() {
  std::string* _s = _internal_mutable_email();
  // @@protoc_insertion_point(field_mutable:game.address_book.Person.email)
  return _s;
}
inline const std::string& Person::_internal_email() const {
  return email_.Get();
}
inline void Person::_internal_set_email(const std::string& value) {
  
  email_.Set(value, GetArenaForAllocation());
}
inline std::string* Person::_internal_mutable_email() {
  
  return email_.Mutable(GetArenaForAllocation());
}
inline std::string* Person::release_email() {
  // @@protoc_insertion_point(field_release:game.address_book.Person.email)
  return email_.Release();
}
inline void Person::set_allocated_email(std::string* email) {
  if (email != nullptr) {
    
  } else {
    
  }
  email_.SetAllocated(email, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (email_.IsDefault()) {
    email_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:game.address_book.Person.email)
}

// repeated .game.address_book.Person.PhoneNumber phones = 4;
inline int Person::_internal_phones_size() const {
  return phones_.size();
}
inline int Person::phones_size() const {
  return _internal_phones_size();
}
inline void Person::clear_phones() {
  phones_.Clear();
}
inline ::game::address_book::Person_PhoneNumber* Person::mutable_phones(int index) {
  // @@protoc_insertion_point(field_mutable:game.address_book.Person.phones)
  return phones_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::game::address_book::Person_PhoneNumber >*
Person::mutable_phones() {
  // @@protoc_insertion_point(field_mutable_list:game.address_book.Person.phones)
  return &phones_;
}
inline const ::game::address_book::Person_PhoneNumber& Person::_internal_phones(int index) const {
  return phones_.Get(index);
}
inline const ::game::address_book::Person_PhoneNumber& Person::phones(int index) const {
  // @@protoc_insertion_point(field_get:game.address_book.Person.phones)
  return _internal_phones(index);
}
inline ::game::address_book::Person_PhoneNumber* Person::_internal_add_phones() {
  return phones_.Add();
}
inline ::game::address_book::Person_PhoneNumber* Person::add_phones() {
  ::game::address_book::Person_PhoneNumber* _add = _internal_add_phones();
  // @@protoc_insertion_point(field_add:game.address_book.Person.phones)
  return _add;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::game::address_book::Person_PhoneNumber >&
Person::phones() const {
  // @@protoc_insertion_point(field_list:game.address_book.Person.phones)
  return phones_;
}

// .google.protobuf.Timestamp last_updated = 5;
inline bool Person::_internal_has_last_updated() const {
  return this != internal_default_instance() && last_updated_ != nullptr;
}
inline bool Person::has_last_updated() const {
  return _internal_has_last_updated();
}
inline const ::PROTOBUF_NAMESPACE_ID::Timestamp& Person::_internal_last_updated() const {
  const ::PROTOBUF_NAMESPACE_ID::Timestamp* p = last_updated_;
  return p != nullptr ? *p : reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Timestamp&>(
      ::PROTOBUF_NAMESPACE_ID::_Timestamp_default_instance_);
}
inline const ::PROTOBUF_NAMESPACE_ID::Timestamp& Person::last_updated() const {
  // @@protoc_insertion_point(field_get:game.address_book.Person.last_updated)
  return _internal_last_updated();
}
inline void Person::unsafe_arena_set_allocated_last_updated(
    ::PROTOBUF_NAMESPACE_ID::Timestamp* last_updated) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(last_updated_);
  }
  last_updated_ = last_updated;
  if (last_updated) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:game.address_book.Person.last_updated)
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* Person::release_last_updated() {
  
  ::PROTOBUF_NAMESPACE_ID::Timestamp* temp = last_updated_;
  last_updated_ = nullptr;
#ifdef PROTOBUF_FORCE_COPY_IN_RELEASE
  auto* old =  reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(temp);
  temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  if (GetArenaForAllocation() == nullptr) { delete old; }
#else  // PROTOBUF_FORCE_COPY_IN_RELEASE
  if (GetArenaForAllocation() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
#endif  // !PROTOBUF_FORCE_COPY_IN_RELEASE
  return temp;
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* Person::unsafe_arena_release_last_updated() {
  // @@protoc_insertion_point(field_release:game.address_book.Person.last_updated)
  
  ::PROTOBUF_NAMESPACE_ID::Timestamp* temp = last_updated_;
  last_updated_ = nullptr;
  return temp;
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* Person::_internal_mutable_last_updated() {
  
  if (last_updated_ == nullptr) {
    auto* p = CreateMaybeMessage<::PROTOBUF_NAMESPACE_ID::Timestamp>(GetArenaForAllocation());
    last_updated_ = p;
  }
  return last_updated_;
}
inline ::PROTOBUF_NAMESPACE_ID::Timestamp* Person::mutable_last_updated() {
  ::PROTOBUF_NAMESPACE_ID::Timestamp* _msg = _internal_mutable_last_updated();
  // @@protoc_insertion_point(field_mutable:game.address_book.Person.last_updated)
  return _msg;
}
inline void Person::set_allocated_last_updated(::PROTOBUF_NAMESPACE_ID::Timestamp* last_updated) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(last_updated_);
  }
  if (last_updated) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(
                reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(last_updated));
    if (message_arena != submessage_arena) {
      last_updated = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, last_updated, submessage_arena);
    }
    
  } else {
    
  }
  last_updated_ = last_updated;
  // @@protoc_insertion_point(field_set_allocated:game.address_book.Person.last_updated)
}

// -------------------------------------------------------------------

// AddressBook

// repeated .game.address_book.Person people = 1;
inline int AddressBook::_internal_people_size() const {
  return people_.size();
}
inline int AddressBook::people_size() const {
  return _internal_people_size();
}
inline void AddressBook::clear_people() {
  people_.Clear();
}
inline ::game::address_book::Person* AddressBook::mutable_people(int index) {
  // @@protoc_insertion_point(field_mutable:game.address_book.AddressBook.people)
  return people_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::game::address_book::Person >*
AddressBook::mutable_people() {
  // @@protoc_insertion_point(field_mutable_list:game.address_book.AddressBook.people)
  return &people_;
}
inline const ::game::address_book::Person& AddressBook::_internal_people(int index) const {
  return people_.Get(index);
}
inline const ::game::address_book::Person& AddressBook::people(int index) const {
  // @@protoc_insertion_point(field_get:game.address_book.AddressBook.people)
  return _internal_people(index);
}
inline ::game::address_book::Person* AddressBook::_internal_add_people() {
  return people_.Add();
}
inline ::game::address_book::Person* AddressBook::add_people() {
  ::game::address_book::Person* _add = _internal_add_people();
  // @@protoc_insertion_point(field_add:game.address_book.AddressBook.people)
  return _add;
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::game::address_book::Person >&
AddressBook::people() const {
  // @@protoc_insertion_point(field_list:game.address_book.AddressBook.people)
  return people_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace address_book
}  // namespace game

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::game::address_book::Person_PhoneType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::game::address_book::Person_PhoneType>() {
  return ::game::address_book::Person_PhoneType_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_addressbook_2faddressbook_2eproto
