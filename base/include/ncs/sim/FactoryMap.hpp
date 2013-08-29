namespace ncs {

namespace sim {

template<template<DeviceType::Type> class Product>
FactoryMap<Product>::FactoryMap(const std::string& product_type)
  : product_type_(product_type) {
}

template<template<DeviceType::Type> class Product>
template<DeviceType::Type MType>
bool FactoryMap<Product>::
register_(const std::string& type,
          std::function<Product<MType>*()> producer,
          std::map<std::string, std::function<Product<MType>*()>>& map) {
  if (map.count(type) != 0) {
    std::cerr << "A " << DeviceType::as_string(MType) << " " << 
      product_type_ << " producer for type " << type <<
      " has already been registered." << std::endl;
    return false;
  }
  map[type] = producer;
}

template<template<DeviceType::Type> class Product>
bool FactoryMap<Product>::registerCUDAProducer(const std::string& type,
                                               CUDAProducer producer) {
  return this->register_<DeviceType::CUDA>(type, producer, cuda_factories_);
}

template<template<DeviceType::Type> class Product>
bool FactoryMap<Product>::registerCPUProducer(const std::string& type,
                                              CPUProducer producer) {
  return this->register_<DeviceType::CPU>(type, producer, cpu_factories_);
}

template<template<DeviceType::Type> class Product>
bool FactoryMap<Product>::registerCLProducer(const std::string& type,
                                             CLProducer producer) {
  return this->register_<DeviceType::CL>(type, producer, cl_factories_);
}

template<template<DeviceType::Type> class Product>
bool FactoryMap<Product>::registerInstantiator(const std::string& type,
                                               Instantiator instantiator) {
  if (instantiators_.count(type) != 0) {
    std::cerr << product_type_ << " Instantiator for type " << type <<
      " was already registered." << std::endl;
    return false;
  }
  instantiators_[type] = instantiator;
  return true;
}

template<template<DeviceType::Type> class Product>
bool FactoryMap<Product>::
registerInstantiatorDestructor(const std::string& type,
                               InstantiatorDestructor destructor) {
  if (destructors_.count(type) != 0) {
    std::cerr << product_type_ << " InstantiatorDestructor for type " <<
      type << "was already registered." << std::endl;
    return false;
  }
  destructors_[type] = destructor;
  return true;
}

template<template<DeviceType::Type> class Product>
typename FactoryMap<Product>::Instantiator
FactoryMap<Product>::getInstantiator(const std::string& type) {
  if (instantiators_.count(type) == 0) {
    std::cerr << product_type_ << " Instantiator for type " << type <<
      " was not registered." << std::endl;
    return Instantiator();
  }
  return instantiators_[type];
}

} // namespace sim

} // namespace ncs
