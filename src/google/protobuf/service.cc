// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#include "google/protobuf/service.h"

#include <functional>
#include <utility>

#include "google/protobuf/stubs/callback.h"
#include "google/protobuf/stubs/common.h"
#include "absl/log/absl_check.h"

namespace google {
namespace protobuf {

Service::~Service() {}
RpcChannel::~RpcChannel() {}
RpcController::~RpcController() {}

namespace {

// A Closure that wraps a std::function<void()> and deletes itself on call.
class FunctionClosure final : public Closure {
 public:
  explicit FunctionClosure(std::function<void()> f) : f_(std::move(f)) {}
  void Run() override {
    f_();
    delete this;
  }

 private:
  std::function<void()> f_;
};

}  // namespace

void Service::CallMethod(const MethodDescriptor* method,
                         RpcController* controller, const Message* request,
                         Message* response, std::function<void()> done) {
  CallMethod(method, controller, request, response,
             service_internal::ToClosure(std::move(done)));
}

void Service::CallMethod(const MethodDescriptor* method,
                         RpcController* controller, const Message* request,
                         Message* response, Closure* done) {
  ABSL_DCHECK_EQ(dynamic_cast<FunctionClosure*>(done), nullptr)
      << "Likely infinite recursion passing std::function "
         "wrapper to default Service::CallMethod. Must implement "
         "one of the overloads.";
  CallMethod(method, controller, request, response,
             service_internal::ToFunction(done));
}

void RpcController::NotifyOnCancel(std::function<void()> callback) {
  NotifyOnCancel(service_internal::ToClosure(std::move(callback)));
}

void RpcController::NotifyOnCancel(Closure* callback) {
  ABSL_DCHECK_EQ(dynamic_cast<FunctionClosure*>(callback), nullptr)
      << "Likely infinite recursion passing std::function "
         "wrapper to default RpcController::NotifyOnCancel. Must "
         "implement one of the overloads.";
  NotifyOnCancel(service_internal::ToFunction(callback));
}

void RpcChannel::CallMethod(const MethodDescriptor* method,
                            RpcController* controller, const Message* request,
                            Message* response, std::function<void()> done) {
  CallMethod(method, controller, request, response,
             service_internal::ToClosure(std::move(done)));
}

void RpcChannel::CallMethod(const MethodDescriptor* method,
                            RpcController* controller, const Message* request,
                            Message* response, Closure* done) {
  ABSL_DCHECK_EQ(dynamic_cast<FunctionClosure*>(done), nullptr)
      << "Likely infinite recursion passing std::function "
         "wrapper to default RpcChannel::CallMethod. Must "
         "implement one of the overloads.";
  CallMethod(method, controller, request, response,
             service_internal::ToFunction(done));
}

namespace service_internal {

Closure* ToClosure(std::function<void()> f) {
  if (f == nullptr) {
    return nullptr;
  }
  return new FunctionClosure(std::move(f));
}

std::function<void()> ToFunction(Closure* callback) {
  if (callback == nullptr) {
    return std::function<void()>();
  }
  return [callback] { callback->Run(); };
}

}  // namespace service_internal
}  // namespace protobuf
}  // namespace google
