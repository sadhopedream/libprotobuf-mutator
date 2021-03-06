// Copyright 2016 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SRC_PROTOBUF_MUTATOR_H_
#define SRC_PROTOBUF_MUTATOR_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <random>
#include <string>

namespace google {
namespace protobuf {
class Message;
}
}

namespace protobuf_mutator {

// Randomly makes incremental change in the given protobuf.
// Usage example:
//    ProtobufMutator mutator(1);
//    MyMessage message;
//    message.ParseFromString(encoded_message);
//    mutator.Mutate(&message, 10000);
//
// Class implements very basic mutations of fields. E.g. it just flips bits for
// integers, floats and strings. Also it increases, decreases size of
// strings only by one. For better results users should override
// ProtobufMutator::Mutate* methods with more useful logic, e.g. using library
// like libFuzzer.
class ProtobufMutator {
 public:
  using RandomEngine = std::mt19937;

  // seed: value to initialize random number generator.
  explicit ProtobufMutator(uint32_t seed);

  // message: message to mutate.
  // size_increase_hint: approximate number of bytes which can be added to the
  // message. Method does not guarantee that real result size increase will be
  // less than the value. It only changes probabilities of mutations which can
  // cause size increase. Caller could repeat mutation if result was larger than
  // requested.
  void Mutate(google::protobuf::Message* message, size_t size_increase_hint);

  // TODO(vitalybuka): implement
  bool CrossOver(const google::protobuf::Message& with,
                 google::protobuf::Message* message);

 protected:
  // TODO(vitalybuka): Consider to replace with single mutate (uint8_t*, size).
  virtual int32_t MutateInt32(int32_t value);
  virtual int64_t MutateInt64(int64_t value);
  virtual uint32_t MutateUInt32(uint32_t value);
  virtual uint64_t MutateUInt64(uint64_t value);
  virtual float MutateFloat(float value);
  virtual double MutateDouble(double value);
  virtual bool MutateBool(bool value);
  virtual size_t MutateEnum(size_t index, size_t item_count);
  virtual std::string MutateString(const std::string& value,
                                   size_t allowed_growth);

  // TODO(vitalybuka): Allow user to control proto level mutations:
  //   * Callbacks to recursive traversal.
  //   * Callbacks for particular proto level mutations.

 private:
  friend class MutateTransformation;
  friend class TestProtobufMutator;
  void InitializeMessage(google::protobuf::Message* message, size_t max_depth);

  bool keep_initialized_ = true;
  RandomEngine random_;
};

}  // namespace protobuf_mutator

#endif  // SRC_PROTOBUF_MUTATOR_H_
