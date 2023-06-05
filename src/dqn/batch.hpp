#pragma once

#include <array>

namespace AppNN {

// template <typename>
// struct ArrayParams;

// template <typename T, size_t N>
// struct ArrayParams<std::array<T, N>> {
//     using ValueType = T;
//     // using Size = N;
//     static size_t const Size = N;
// };

template <typename, size_t>
class Batch;

template <typename ObjectValueType, size_t ObjectSize, template <typename, size_t> class Object, size_t BatchSize>
class Batch<Object<ObjectValueType, ObjectSize>, BatchSize> {
public:
    Batch() = default;

    void SetObject(int index, Object<ObjectValueType, ObjectSize> object) {
        std::copy(object.begin(), object.end(), data_ + BatchSize * index);
    }
    
    torch::Tensor ToDevice() {
        return torch::from_blob(data_, {BatchSize, states[0].size()}).clone().to(device);
    }

private:
    ObjectValueType data_[BatchSize][ObjectSize];
};

} // namespace AppNN