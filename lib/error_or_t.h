#pragma once
#include <memory>
#include <string>

template <typename T>
class ErrorOr {
  public:
   const std::shared_ptr<T> data;
   const std::string error;

   bool successed() const { return data.get() != nullptr; }
   bool failed() const { return !successed(); }

   ErrorOr(const std::shared_ptr<T> data, const std::string error) : data(data), error(error) {}

   static ErrorOr withSuccess(std::shared_ptr<T> data) { return ErrorOr(data, ""); }

   static ErrorOr withError(const std::string error) { return ErrorOr(nullptr, error); }
};
