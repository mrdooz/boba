// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: effect_settings_generator.proto

#ifndef PROTOBUF_effect_5fsettings_5fgenerator_2eproto__INCLUDED
#define PROTOBUF_effect_5fsettings_5fgenerator_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
#include "anttweak.pb.h"
#include "common.pb.h"
// @@protoc_insertion_point(includes)

namespace protocol {
namespace effect {
namespace generator {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_effect_5fsettings_5fgenerator_2eproto();
void protobuf_AssignDesc_effect_5fsettings_5fgenerator_2eproto();
void protobuf_ShutdownFile_effect_5fsettings_5fgenerator_2eproto();

class SpikyConfig;
class PlaneConfig;

// ===================================================================

class SpikyConfig : public ::google::protobuf::Message {
 public:
  SpikyConfig();
  virtual ~SpikyConfig();

  SpikyConfig(const SpikyConfig& from);

  inline SpikyConfig& operator=(const SpikyConfig& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const SpikyConfig& default_instance();

  void Swap(SpikyConfig* other);

  // implements Message ----------------------------------------------

  SpikyConfig* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const SpikyConfig& from);
  void MergeFrom(const SpikyConfig& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional float radius = 1;
  inline bool has_radius() const;
  inline void clear_radius();
  static const int kRadiusFieldNumber = 1;
  inline float radius() const;
  inline void set_radius(float value);

  // optional float height = 2;
  inline bool has_height() const;
  inline void clear_height();
  static const int kHeightFieldNumber = 2;
  inline float height() const;
  inline void set_height(float value);

  // optional uint32 radial_segments = 3;
  inline bool has_radial_segments() const;
  inline void clear_radial_segments();
  static const int kRadialSegmentsFieldNumber = 3;
  inline ::google::protobuf::uint32 radial_segments() const;
  inline void set_radial_segments(::google::protobuf::uint32 value);

  // optional uint32 height_segments = 4;
  inline bool has_height_segments() const;
  inline void clear_height_segments();
  static const int kHeightSegmentsFieldNumber = 4;
  inline ::google::protobuf::uint32 height_segments() const;
  inline void set_height_segments(::google::protobuf::uint32 value);

  // optional .protocol.common.Vector3 camera_pos = 5;
  inline bool has_camera_pos() const;
  inline void clear_camera_pos();
  static const int kCameraPosFieldNumber = 5;
  inline const ::protocol::common::Vector3& camera_pos() const;
  inline ::protocol::common::Vector3* mutable_camera_pos();
  inline ::protocol::common::Vector3* release_camera_pos();
  inline void set_allocated_camera_pos(::protocol::common::Vector3* camera_pos);

  // optional .protocol.common.Vector3 camera_dir = 6;
  inline bool has_camera_dir() const;
  inline void clear_camera_dir();
  static const int kCameraDirFieldNumber = 6;
  inline const ::protocol::common::Vector3& camera_dir() const;
  inline ::protocol::common::Vector3* mutable_camera_dir();
  inline ::protocol::common::Vector3* release_camera_dir();
  inline void set_allocated_camera_dir(::protocol::common::Vector3* camera_dir);

  // optional .protocol.common.Vector3 obj_t = 7;
  inline bool has_obj_t() const;
  inline void clear_obj_t();
  static const int kObjTFieldNumber = 7;
  inline const ::protocol::common::Vector3& obj_t() const;
  inline ::protocol::common::Vector3* mutable_obj_t();
  inline ::protocol::common::Vector3* release_obj_t();
  inline void set_allocated_obj_t(::protocol::common::Vector3* obj_t);

  // optional .protocol.common.Matrix4x4 obj_r = 8;
  inline bool has_obj_r() const;
  inline void clear_obj_r();
  static const int kObjRFieldNumber = 8;
  inline const ::protocol::common::Matrix4x4& obj_r() const;
  inline ::protocol::common::Matrix4x4* mutable_obj_r();
  inline ::protocol::common::Matrix4x4* release_obj_r();
  inline void set_allocated_obj_r(::protocol::common::Matrix4x4* obj_r);

  // @@protoc_insertion_point(class_scope:protocol.effect.generator.SpikyConfig)
 private:
  inline void set_has_radius();
  inline void clear_has_radius();
  inline void set_has_height();
  inline void clear_has_height();
  inline void set_has_radial_segments();
  inline void clear_has_radial_segments();
  inline void set_has_height_segments();
  inline void clear_has_height_segments();
  inline void set_has_camera_pos();
  inline void clear_has_camera_pos();
  inline void set_has_camera_dir();
  inline void clear_has_camera_dir();
  inline void set_has_obj_t();
  inline void clear_has_obj_t();
  inline void set_has_obj_r();
  inline void clear_has_obj_r();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  float radius_;
  float height_;
  ::google::protobuf::uint32 radial_segments_;
  ::google::protobuf::uint32 height_segments_;
  ::protocol::common::Vector3* camera_pos_;
  ::protocol::common::Vector3* camera_dir_;
  ::protocol::common::Vector3* obj_t_;
  ::protocol::common::Matrix4x4* obj_r_;
  friend void  protobuf_AddDesc_effect_5fsettings_5fgenerator_2eproto();
  friend void protobuf_AssignDesc_effect_5fsettings_5fgenerator_2eproto();
  friend void protobuf_ShutdownFile_effect_5fsettings_5fgenerator_2eproto();

  void InitAsDefaultInstance();
  static SpikyConfig* default_instance_;
};
// -------------------------------------------------------------------

class PlaneConfig : public ::google::protobuf::Message {
 public:
  PlaneConfig();
  virtual ~PlaneConfig();

  PlaneConfig(const PlaneConfig& from);

  inline PlaneConfig& operator=(const PlaneConfig& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const PlaneConfig& default_instance();

  void Swap(PlaneConfig* other);

  // implements Message ----------------------------------------------

  PlaneConfig* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const PlaneConfig& from);
  void MergeFrom(const PlaneConfig& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional float width = 1;
  inline bool has_width() const;
  inline void clear_width();
  static const int kWidthFieldNumber = 1;
  inline float width() const;
  inline void set_width(float value);

  // optional float height = 2;
  inline bool has_height() const;
  inline void clear_height();
  static const int kHeightFieldNumber = 2;
  inline float height() const;
  inline void set_height(float value);

  // optional uint32 width_segments = 3;
  inline bool has_width_segments() const;
  inline void clear_width_segments();
  static const int kWidthSegmentsFieldNumber = 3;
  inline ::google::protobuf::uint32 width_segments() const;
  inline void set_width_segments(::google::protobuf::uint32 value);

  // optional uint32 height_segments = 4;
  inline bool has_height_segments() const;
  inline void clear_height_segments();
  static const int kHeightSegmentsFieldNumber = 4;
  inline ::google::protobuf::uint32 height_segments() const;
  inline void set_height_segments(::google::protobuf::uint32 value);

  // optional .protocol.common.Vector3 camera_pos = 5;
  inline bool has_camera_pos() const;
  inline void clear_camera_pos();
  static const int kCameraPosFieldNumber = 5;
  inline const ::protocol::common::Vector3& camera_pos() const;
  inline ::protocol::common::Vector3* mutable_camera_pos();
  inline ::protocol::common::Vector3* release_camera_pos();
  inline void set_allocated_camera_pos(::protocol::common::Vector3* camera_pos);

  // optional .protocol.common.Vector3 camera_dir = 6;
  inline bool has_camera_dir() const;
  inline void clear_camera_dir();
  static const int kCameraDirFieldNumber = 6;
  inline const ::protocol::common::Vector3& camera_dir() const;
  inline ::protocol::common::Vector3* mutable_camera_dir();
  inline ::protocol::common::Vector3* release_camera_dir();
  inline void set_allocated_camera_dir(::protocol::common::Vector3* camera_dir);

  // optional .protocol.common.Vector3 obj_t = 7;
  inline bool has_obj_t() const;
  inline void clear_obj_t();
  static const int kObjTFieldNumber = 7;
  inline const ::protocol::common::Vector3& obj_t() const;
  inline ::protocol::common::Vector3* mutable_obj_t();
  inline ::protocol::common::Vector3* release_obj_t();
  inline void set_allocated_obj_t(::protocol::common::Vector3* obj_t);

  // optional .protocol.common.Matrix4x4 obj_r = 8;
  inline bool has_obj_r() const;
  inline void clear_obj_r();
  static const int kObjRFieldNumber = 8;
  inline const ::protocol::common::Matrix4x4& obj_r() const;
  inline ::protocol::common::Matrix4x4* mutable_obj_r();
  inline ::protocol::common::Matrix4x4* release_obj_r();
  inline void set_allocated_obj_r(::protocol::common::Matrix4x4* obj_r);

  // optional float tau = 9 [default = 0.5];
  inline bool has_tau() const;
  inline void clear_tau();
  static const int kTauFieldNumber = 9;
  inline float tau() const;
  inline void set_tau(float value);

  // optional float key = 10 [default = 0.18];
  inline bool has_key() const;
  inline void clear_key();
  static const int kKeyFieldNumber = 10;
  inline float key() const;
  inline void set_key(float value);

  // optional float ofs = 11 [default = 0.18];
  inline bool has_ofs() const;
  inline void clear_ofs();
  static const int kOfsFieldNumber = 11;
  inline float ofs() const;
  inline void set_ofs(float value);

  // optional float blur_radius = 12 [default = 10];
  inline bool has_blur_radius() const;
  inline void clear_blur_radius();
  static const int kBlurRadiusFieldNumber = 12;
  inline float blur_radius() const;
  inline void set_blur_radius(float value);

  // optional float bloom_threshold = 13 [default = 0.8];
  inline bool has_bloom_threshold() const;
  inline void clear_bloom_threshold();
  static const int kBloomThresholdFieldNumber = 13;
  inline float bloom_threshold() const;
  inline void set_bloom_threshold(float value);

  // optional float bloom_multiplier = 14 [default = 1];
  inline bool has_bloom_multiplier() const;
  inline void clear_bloom_multiplier();
  static const int kBloomMultiplierFieldNumber = 14;
  inline float bloom_multiplier() const;
  inline void set_bloom_multiplier(float value);

  // optional bool transpose = 15 [default = true];
  inline bool has_transpose() const;
  inline void clear_transpose();
  static const int kTransposeFieldNumber = 15;
  inline bool transpose() const;
  inline void set_transpose(bool value);

  // @@protoc_insertion_point(class_scope:protocol.effect.generator.PlaneConfig)
 private:
  inline void set_has_width();
  inline void clear_has_width();
  inline void set_has_height();
  inline void clear_has_height();
  inline void set_has_width_segments();
  inline void clear_has_width_segments();
  inline void set_has_height_segments();
  inline void clear_has_height_segments();
  inline void set_has_camera_pos();
  inline void clear_has_camera_pos();
  inline void set_has_camera_dir();
  inline void clear_has_camera_dir();
  inline void set_has_obj_t();
  inline void clear_has_obj_t();
  inline void set_has_obj_r();
  inline void clear_has_obj_r();
  inline void set_has_tau();
  inline void clear_has_tau();
  inline void set_has_key();
  inline void clear_has_key();
  inline void set_has_ofs();
  inline void clear_has_ofs();
  inline void set_has_blur_radius();
  inline void clear_has_blur_radius();
  inline void set_has_bloom_threshold();
  inline void clear_has_bloom_threshold();
  inline void set_has_bloom_multiplier();
  inline void clear_has_bloom_multiplier();
  inline void set_has_transpose();
  inline void clear_has_transpose();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  float width_;
  float height_;
  ::google::protobuf::uint32 width_segments_;
  ::google::protobuf::uint32 height_segments_;
  ::protocol::common::Vector3* camera_pos_;
  ::protocol::common::Vector3* camera_dir_;
  ::protocol::common::Vector3* obj_t_;
  ::protocol::common::Matrix4x4* obj_r_;
  float tau_;
  float key_;
  float ofs_;
  float blur_radius_;
  float bloom_threshold_;
  float bloom_multiplier_;
  bool transpose_;
  friend void  protobuf_AddDesc_effect_5fsettings_5fgenerator_2eproto();
  friend void protobuf_AssignDesc_effect_5fsettings_5fgenerator_2eproto();
  friend void protobuf_ShutdownFile_effect_5fsettings_5fgenerator_2eproto();

  void InitAsDefaultInstance();
  static PlaneConfig* default_instance_;
};
// ===================================================================


// ===================================================================

// SpikyConfig

// optional float radius = 1;
inline bool SpikyConfig::has_radius() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void SpikyConfig::set_has_radius() {
  _has_bits_[0] |= 0x00000001u;
}
inline void SpikyConfig::clear_has_radius() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void SpikyConfig::clear_radius() {
  radius_ = 0;
  clear_has_radius();
}
inline float SpikyConfig::radius() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.SpikyConfig.radius)
  return radius_;
}
inline void SpikyConfig::set_radius(float value) {
  set_has_radius();
  radius_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.SpikyConfig.radius)
}

// optional float height = 2;
inline bool SpikyConfig::has_height() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void SpikyConfig::set_has_height() {
  _has_bits_[0] |= 0x00000002u;
}
inline void SpikyConfig::clear_has_height() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void SpikyConfig::clear_height() {
  height_ = 0;
  clear_has_height();
}
inline float SpikyConfig::height() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.SpikyConfig.height)
  return height_;
}
inline void SpikyConfig::set_height(float value) {
  set_has_height();
  height_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.SpikyConfig.height)
}

// optional uint32 radial_segments = 3;
inline bool SpikyConfig::has_radial_segments() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void SpikyConfig::set_has_radial_segments() {
  _has_bits_[0] |= 0x00000004u;
}
inline void SpikyConfig::clear_has_radial_segments() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void SpikyConfig::clear_radial_segments() {
  radial_segments_ = 0u;
  clear_has_radial_segments();
}
inline ::google::protobuf::uint32 SpikyConfig::radial_segments() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.SpikyConfig.radial_segments)
  return radial_segments_;
}
inline void SpikyConfig::set_radial_segments(::google::protobuf::uint32 value) {
  set_has_radial_segments();
  radial_segments_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.SpikyConfig.radial_segments)
}

// optional uint32 height_segments = 4;
inline bool SpikyConfig::has_height_segments() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void SpikyConfig::set_has_height_segments() {
  _has_bits_[0] |= 0x00000008u;
}
inline void SpikyConfig::clear_has_height_segments() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void SpikyConfig::clear_height_segments() {
  height_segments_ = 0u;
  clear_has_height_segments();
}
inline ::google::protobuf::uint32 SpikyConfig::height_segments() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.SpikyConfig.height_segments)
  return height_segments_;
}
inline void SpikyConfig::set_height_segments(::google::protobuf::uint32 value) {
  set_has_height_segments();
  height_segments_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.SpikyConfig.height_segments)
}

// optional .protocol.common.Vector3 camera_pos = 5;
inline bool SpikyConfig::has_camera_pos() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void SpikyConfig::set_has_camera_pos() {
  _has_bits_[0] |= 0x00000010u;
}
inline void SpikyConfig::clear_has_camera_pos() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void SpikyConfig::clear_camera_pos() {
  if (camera_pos_ != NULL) camera_pos_->::protocol::common::Vector3::Clear();
  clear_has_camera_pos();
}
inline const ::protocol::common::Vector3& SpikyConfig::camera_pos() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.SpikyConfig.camera_pos)
  return camera_pos_ != NULL ? *camera_pos_ : *default_instance_->camera_pos_;
}
inline ::protocol::common::Vector3* SpikyConfig::mutable_camera_pos() {
  set_has_camera_pos();
  if (camera_pos_ == NULL) camera_pos_ = new ::protocol::common::Vector3;
  // @@protoc_insertion_point(field_mutable:protocol.effect.generator.SpikyConfig.camera_pos)
  return camera_pos_;
}
inline ::protocol::common::Vector3* SpikyConfig::release_camera_pos() {
  clear_has_camera_pos();
  ::protocol::common::Vector3* temp = camera_pos_;
  camera_pos_ = NULL;
  return temp;
}
inline void SpikyConfig::set_allocated_camera_pos(::protocol::common::Vector3* camera_pos) {
  delete camera_pos_;
  camera_pos_ = camera_pos;
  if (camera_pos) {
    set_has_camera_pos();
  } else {
    clear_has_camera_pos();
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.effect.generator.SpikyConfig.camera_pos)
}

// optional .protocol.common.Vector3 camera_dir = 6;
inline bool SpikyConfig::has_camera_dir() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void SpikyConfig::set_has_camera_dir() {
  _has_bits_[0] |= 0x00000020u;
}
inline void SpikyConfig::clear_has_camera_dir() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void SpikyConfig::clear_camera_dir() {
  if (camera_dir_ != NULL) camera_dir_->::protocol::common::Vector3::Clear();
  clear_has_camera_dir();
}
inline const ::protocol::common::Vector3& SpikyConfig::camera_dir() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.SpikyConfig.camera_dir)
  return camera_dir_ != NULL ? *camera_dir_ : *default_instance_->camera_dir_;
}
inline ::protocol::common::Vector3* SpikyConfig::mutable_camera_dir() {
  set_has_camera_dir();
  if (camera_dir_ == NULL) camera_dir_ = new ::protocol::common::Vector3;
  // @@protoc_insertion_point(field_mutable:protocol.effect.generator.SpikyConfig.camera_dir)
  return camera_dir_;
}
inline ::protocol::common::Vector3* SpikyConfig::release_camera_dir() {
  clear_has_camera_dir();
  ::protocol::common::Vector3* temp = camera_dir_;
  camera_dir_ = NULL;
  return temp;
}
inline void SpikyConfig::set_allocated_camera_dir(::protocol::common::Vector3* camera_dir) {
  delete camera_dir_;
  camera_dir_ = camera_dir;
  if (camera_dir) {
    set_has_camera_dir();
  } else {
    clear_has_camera_dir();
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.effect.generator.SpikyConfig.camera_dir)
}

// optional .protocol.common.Vector3 obj_t = 7;
inline bool SpikyConfig::has_obj_t() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void SpikyConfig::set_has_obj_t() {
  _has_bits_[0] |= 0x00000040u;
}
inline void SpikyConfig::clear_has_obj_t() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void SpikyConfig::clear_obj_t() {
  if (obj_t_ != NULL) obj_t_->::protocol::common::Vector3::Clear();
  clear_has_obj_t();
}
inline const ::protocol::common::Vector3& SpikyConfig::obj_t() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.SpikyConfig.obj_t)
  return obj_t_ != NULL ? *obj_t_ : *default_instance_->obj_t_;
}
inline ::protocol::common::Vector3* SpikyConfig::mutable_obj_t() {
  set_has_obj_t();
  if (obj_t_ == NULL) obj_t_ = new ::protocol::common::Vector3;
  // @@protoc_insertion_point(field_mutable:protocol.effect.generator.SpikyConfig.obj_t)
  return obj_t_;
}
inline ::protocol::common::Vector3* SpikyConfig::release_obj_t() {
  clear_has_obj_t();
  ::protocol::common::Vector3* temp = obj_t_;
  obj_t_ = NULL;
  return temp;
}
inline void SpikyConfig::set_allocated_obj_t(::protocol::common::Vector3* obj_t) {
  delete obj_t_;
  obj_t_ = obj_t;
  if (obj_t) {
    set_has_obj_t();
  } else {
    clear_has_obj_t();
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.effect.generator.SpikyConfig.obj_t)
}

// optional .protocol.common.Matrix4x4 obj_r = 8;
inline bool SpikyConfig::has_obj_r() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void SpikyConfig::set_has_obj_r() {
  _has_bits_[0] |= 0x00000080u;
}
inline void SpikyConfig::clear_has_obj_r() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void SpikyConfig::clear_obj_r() {
  if (obj_r_ != NULL) obj_r_->::protocol::common::Matrix4x4::Clear();
  clear_has_obj_r();
}
inline const ::protocol::common::Matrix4x4& SpikyConfig::obj_r() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.SpikyConfig.obj_r)
  return obj_r_ != NULL ? *obj_r_ : *default_instance_->obj_r_;
}
inline ::protocol::common::Matrix4x4* SpikyConfig::mutable_obj_r() {
  set_has_obj_r();
  if (obj_r_ == NULL) obj_r_ = new ::protocol::common::Matrix4x4;
  // @@protoc_insertion_point(field_mutable:protocol.effect.generator.SpikyConfig.obj_r)
  return obj_r_;
}
inline ::protocol::common::Matrix4x4* SpikyConfig::release_obj_r() {
  clear_has_obj_r();
  ::protocol::common::Matrix4x4* temp = obj_r_;
  obj_r_ = NULL;
  return temp;
}
inline void SpikyConfig::set_allocated_obj_r(::protocol::common::Matrix4x4* obj_r) {
  delete obj_r_;
  obj_r_ = obj_r;
  if (obj_r) {
    set_has_obj_r();
  } else {
    clear_has_obj_r();
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.effect.generator.SpikyConfig.obj_r)
}

// -------------------------------------------------------------------

// PlaneConfig

// optional float width = 1;
inline bool PlaneConfig::has_width() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void PlaneConfig::set_has_width() {
  _has_bits_[0] |= 0x00000001u;
}
inline void PlaneConfig::clear_has_width() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void PlaneConfig::clear_width() {
  width_ = 0;
  clear_has_width();
}
inline float PlaneConfig::width() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.width)
  return width_;
}
inline void PlaneConfig::set_width(float value) {
  set_has_width();
  width_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.PlaneConfig.width)
}

// optional float height = 2;
inline bool PlaneConfig::has_height() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void PlaneConfig::set_has_height() {
  _has_bits_[0] |= 0x00000002u;
}
inline void PlaneConfig::clear_has_height() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void PlaneConfig::clear_height() {
  height_ = 0;
  clear_has_height();
}
inline float PlaneConfig::height() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.height)
  return height_;
}
inline void PlaneConfig::set_height(float value) {
  set_has_height();
  height_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.PlaneConfig.height)
}

// optional uint32 width_segments = 3;
inline bool PlaneConfig::has_width_segments() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void PlaneConfig::set_has_width_segments() {
  _has_bits_[0] |= 0x00000004u;
}
inline void PlaneConfig::clear_has_width_segments() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void PlaneConfig::clear_width_segments() {
  width_segments_ = 0u;
  clear_has_width_segments();
}
inline ::google::protobuf::uint32 PlaneConfig::width_segments() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.width_segments)
  return width_segments_;
}
inline void PlaneConfig::set_width_segments(::google::protobuf::uint32 value) {
  set_has_width_segments();
  width_segments_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.PlaneConfig.width_segments)
}

// optional uint32 height_segments = 4;
inline bool PlaneConfig::has_height_segments() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void PlaneConfig::set_has_height_segments() {
  _has_bits_[0] |= 0x00000008u;
}
inline void PlaneConfig::clear_has_height_segments() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void PlaneConfig::clear_height_segments() {
  height_segments_ = 0u;
  clear_has_height_segments();
}
inline ::google::protobuf::uint32 PlaneConfig::height_segments() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.height_segments)
  return height_segments_;
}
inline void PlaneConfig::set_height_segments(::google::protobuf::uint32 value) {
  set_has_height_segments();
  height_segments_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.PlaneConfig.height_segments)
}

// optional .protocol.common.Vector3 camera_pos = 5;
inline bool PlaneConfig::has_camera_pos() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void PlaneConfig::set_has_camera_pos() {
  _has_bits_[0] |= 0x00000010u;
}
inline void PlaneConfig::clear_has_camera_pos() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void PlaneConfig::clear_camera_pos() {
  if (camera_pos_ != NULL) camera_pos_->::protocol::common::Vector3::Clear();
  clear_has_camera_pos();
}
inline const ::protocol::common::Vector3& PlaneConfig::camera_pos() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.camera_pos)
  return camera_pos_ != NULL ? *camera_pos_ : *default_instance_->camera_pos_;
}
inline ::protocol::common::Vector3* PlaneConfig::mutable_camera_pos() {
  set_has_camera_pos();
  if (camera_pos_ == NULL) camera_pos_ = new ::protocol::common::Vector3;
  // @@protoc_insertion_point(field_mutable:protocol.effect.generator.PlaneConfig.camera_pos)
  return camera_pos_;
}
inline ::protocol::common::Vector3* PlaneConfig::release_camera_pos() {
  clear_has_camera_pos();
  ::protocol::common::Vector3* temp = camera_pos_;
  camera_pos_ = NULL;
  return temp;
}
inline void PlaneConfig::set_allocated_camera_pos(::protocol::common::Vector3* camera_pos) {
  delete camera_pos_;
  camera_pos_ = camera_pos;
  if (camera_pos) {
    set_has_camera_pos();
  } else {
    clear_has_camera_pos();
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.effect.generator.PlaneConfig.camera_pos)
}

// optional .protocol.common.Vector3 camera_dir = 6;
inline bool PlaneConfig::has_camera_dir() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void PlaneConfig::set_has_camera_dir() {
  _has_bits_[0] |= 0x00000020u;
}
inline void PlaneConfig::clear_has_camera_dir() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void PlaneConfig::clear_camera_dir() {
  if (camera_dir_ != NULL) camera_dir_->::protocol::common::Vector3::Clear();
  clear_has_camera_dir();
}
inline const ::protocol::common::Vector3& PlaneConfig::camera_dir() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.camera_dir)
  return camera_dir_ != NULL ? *camera_dir_ : *default_instance_->camera_dir_;
}
inline ::protocol::common::Vector3* PlaneConfig::mutable_camera_dir() {
  set_has_camera_dir();
  if (camera_dir_ == NULL) camera_dir_ = new ::protocol::common::Vector3;
  // @@protoc_insertion_point(field_mutable:protocol.effect.generator.PlaneConfig.camera_dir)
  return camera_dir_;
}
inline ::protocol::common::Vector3* PlaneConfig::release_camera_dir() {
  clear_has_camera_dir();
  ::protocol::common::Vector3* temp = camera_dir_;
  camera_dir_ = NULL;
  return temp;
}
inline void PlaneConfig::set_allocated_camera_dir(::protocol::common::Vector3* camera_dir) {
  delete camera_dir_;
  camera_dir_ = camera_dir;
  if (camera_dir) {
    set_has_camera_dir();
  } else {
    clear_has_camera_dir();
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.effect.generator.PlaneConfig.camera_dir)
}

// optional .protocol.common.Vector3 obj_t = 7;
inline bool PlaneConfig::has_obj_t() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void PlaneConfig::set_has_obj_t() {
  _has_bits_[0] |= 0x00000040u;
}
inline void PlaneConfig::clear_has_obj_t() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void PlaneConfig::clear_obj_t() {
  if (obj_t_ != NULL) obj_t_->::protocol::common::Vector3::Clear();
  clear_has_obj_t();
}
inline const ::protocol::common::Vector3& PlaneConfig::obj_t() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.obj_t)
  return obj_t_ != NULL ? *obj_t_ : *default_instance_->obj_t_;
}
inline ::protocol::common::Vector3* PlaneConfig::mutable_obj_t() {
  set_has_obj_t();
  if (obj_t_ == NULL) obj_t_ = new ::protocol::common::Vector3;
  // @@protoc_insertion_point(field_mutable:protocol.effect.generator.PlaneConfig.obj_t)
  return obj_t_;
}
inline ::protocol::common::Vector3* PlaneConfig::release_obj_t() {
  clear_has_obj_t();
  ::protocol::common::Vector3* temp = obj_t_;
  obj_t_ = NULL;
  return temp;
}
inline void PlaneConfig::set_allocated_obj_t(::protocol::common::Vector3* obj_t) {
  delete obj_t_;
  obj_t_ = obj_t;
  if (obj_t) {
    set_has_obj_t();
  } else {
    clear_has_obj_t();
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.effect.generator.PlaneConfig.obj_t)
}

// optional .protocol.common.Matrix4x4 obj_r = 8;
inline bool PlaneConfig::has_obj_r() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void PlaneConfig::set_has_obj_r() {
  _has_bits_[0] |= 0x00000080u;
}
inline void PlaneConfig::clear_has_obj_r() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void PlaneConfig::clear_obj_r() {
  if (obj_r_ != NULL) obj_r_->::protocol::common::Matrix4x4::Clear();
  clear_has_obj_r();
}
inline const ::protocol::common::Matrix4x4& PlaneConfig::obj_r() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.obj_r)
  return obj_r_ != NULL ? *obj_r_ : *default_instance_->obj_r_;
}
inline ::protocol::common::Matrix4x4* PlaneConfig::mutable_obj_r() {
  set_has_obj_r();
  if (obj_r_ == NULL) obj_r_ = new ::protocol::common::Matrix4x4;
  // @@protoc_insertion_point(field_mutable:protocol.effect.generator.PlaneConfig.obj_r)
  return obj_r_;
}
inline ::protocol::common::Matrix4x4* PlaneConfig::release_obj_r() {
  clear_has_obj_r();
  ::protocol::common::Matrix4x4* temp = obj_r_;
  obj_r_ = NULL;
  return temp;
}
inline void PlaneConfig::set_allocated_obj_r(::protocol::common::Matrix4x4* obj_r) {
  delete obj_r_;
  obj_r_ = obj_r;
  if (obj_r) {
    set_has_obj_r();
  } else {
    clear_has_obj_r();
  }
  // @@protoc_insertion_point(field_set_allocated:protocol.effect.generator.PlaneConfig.obj_r)
}

// optional float tau = 9 [default = 0.5];
inline bool PlaneConfig::has_tau() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void PlaneConfig::set_has_tau() {
  _has_bits_[0] |= 0x00000100u;
}
inline void PlaneConfig::clear_has_tau() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void PlaneConfig::clear_tau() {
  tau_ = 0.5f;
  clear_has_tau();
}
inline float PlaneConfig::tau() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.tau)
  return tau_;
}
inline void PlaneConfig::set_tau(float value) {
  set_has_tau();
  tau_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.PlaneConfig.tau)
}

// optional float key = 10 [default = 0.18];
inline bool PlaneConfig::has_key() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void PlaneConfig::set_has_key() {
  _has_bits_[0] |= 0x00000200u;
}
inline void PlaneConfig::clear_has_key() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void PlaneConfig::clear_key() {
  key_ = 0.18f;
  clear_has_key();
}
inline float PlaneConfig::key() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.key)
  return key_;
}
inline void PlaneConfig::set_key(float value) {
  set_has_key();
  key_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.PlaneConfig.key)
}

// optional float ofs = 11 [default = 0.18];
inline bool PlaneConfig::has_ofs() const {
  return (_has_bits_[0] & 0x00000400u) != 0;
}
inline void PlaneConfig::set_has_ofs() {
  _has_bits_[0] |= 0x00000400u;
}
inline void PlaneConfig::clear_has_ofs() {
  _has_bits_[0] &= ~0x00000400u;
}
inline void PlaneConfig::clear_ofs() {
  ofs_ = 0.18f;
  clear_has_ofs();
}
inline float PlaneConfig::ofs() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.ofs)
  return ofs_;
}
inline void PlaneConfig::set_ofs(float value) {
  set_has_ofs();
  ofs_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.PlaneConfig.ofs)
}

// optional float blur_radius = 12 [default = 10];
inline bool PlaneConfig::has_blur_radius() const {
  return (_has_bits_[0] & 0x00000800u) != 0;
}
inline void PlaneConfig::set_has_blur_radius() {
  _has_bits_[0] |= 0x00000800u;
}
inline void PlaneConfig::clear_has_blur_radius() {
  _has_bits_[0] &= ~0x00000800u;
}
inline void PlaneConfig::clear_blur_radius() {
  blur_radius_ = 10;
  clear_has_blur_radius();
}
inline float PlaneConfig::blur_radius() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.blur_radius)
  return blur_radius_;
}
inline void PlaneConfig::set_blur_radius(float value) {
  set_has_blur_radius();
  blur_radius_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.PlaneConfig.blur_radius)
}

// optional float bloom_threshold = 13 [default = 0.8];
inline bool PlaneConfig::has_bloom_threshold() const {
  return (_has_bits_[0] & 0x00001000u) != 0;
}
inline void PlaneConfig::set_has_bloom_threshold() {
  _has_bits_[0] |= 0x00001000u;
}
inline void PlaneConfig::clear_has_bloom_threshold() {
  _has_bits_[0] &= ~0x00001000u;
}
inline void PlaneConfig::clear_bloom_threshold() {
  bloom_threshold_ = 0.8f;
  clear_has_bloom_threshold();
}
inline float PlaneConfig::bloom_threshold() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.bloom_threshold)
  return bloom_threshold_;
}
inline void PlaneConfig::set_bloom_threshold(float value) {
  set_has_bloom_threshold();
  bloom_threshold_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.PlaneConfig.bloom_threshold)
}

// optional float bloom_multiplier = 14 [default = 1];
inline bool PlaneConfig::has_bloom_multiplier() const {
  return (_has_bits_[0] & 0x00002000u) != 0;
}
inline void PlaneConfig::set_has_bloom_multiplier() {
  _has_bits_[0] |= 0x00002000u;
}
inline void PlaneConfig::clear_has_bloom_multiplier() {
  _has_bits_[0] &= ~0x00002000u;
}
inline void PlaneConfig::clear_bloom_multiplier() {
  bloom_multiplier_ = 1;
  clear_has_bloom_multiplier();
}
inline float PlaneConfig::bloom_multiplier() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.bloom_multiplier)
  return bloom_multiplier_;
}
inline void PlaneConfig::set_bloom_multiplier(float value) {
  set_has_bloom_multiplier();
  bloom_multiplier_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.PlaneConfig.bloom_multiplier)
}

// optional bool transpose = 15 [default = true];
inline bool PlaneConfig::has_transpose() const {
  return (_has_bits_[0] & 0x00004000u) != 0;
}
inline void PlaneConfig::set_has_transpose() {
  _has_bits_[0] |= 0x00004000u;
}
inline void PlaneConfig::clear_has_transpose() {
  _has_bits_[0] &= ~0x00004000u;
}
inline void PlaneConfig::clear_transpose() {
  transpose_ = true;
  clear_has_transpose();
}
inline bool PlaneConfig::transpose() const {
  // @@protoc_insertion_point(field_get:protocol.effect.generator.PlaneConfig.transpose)
  return transpose_;
}
inline void PlaneConfig::set_transpose(bool value) {
  set_has_transpose();
  transpose_ = value;
  // @@protoc_insertion_point(field_set:protocol.effect.generator.PlaneConfig.transpose)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace generator
}  // namespace effect
}  // namespace protocol

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_effect_5fsettings_5fgenerator_2eproto__INCLUDED
