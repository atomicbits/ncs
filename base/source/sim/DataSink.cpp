#include <ncs/sim/DataSink.h>

namespace ncs {

namespace sim {

DataSinkBuffer::DataSinkBuffer()
  : data_(nullptr) {
}

void DataSinkBuffer::setData(const void* data) {
  data_ = data;
}

const void* DataSinkBuffer::getData() const {
  return data_;
}

DataSinkBuffer::~DataSinkBuffer() {
}

DataSink::DataSink(const DataDescription* data_description,
                   size_t num_padding_elements,
                   size_t num_real_elements,
                   size_t num_buffers)
  : data_description_(new DataDescription(*data_description)),
    num_padding_elements_(num_padding_elements),
    num_real_elements_(num_real_elements),
    num_buffers_(num_buffers),
    source_subscription_(nullptr) {
  num_total_elements_ = num_padding_elements_ + num_real_elements_;
}

bool DataSink::init(const std::vector<SpecificPublisher<Signal>*> dependents,
                    ReportController* report_controller) {
  size_t data_size = DataType::num_bytes(num_total_elements_,
                                         data_description_->getDataType());
  if (data_size <= 0) {
    std::cerr << "No data is actually collected in this sink." << std::endl;
    return false;
  }
  if (num_buffers_ <= 0) {
    std::cerr << "num_buffers must be greater than 0 for DataSink." <<
      std::endl;
    return false;
  }
  for (size_t i = 0; i < num_buffers_; ++i) {
    DataSinkBuffer* buffer = new DataSinkBuffer();
    addBlank(buffer);
  }
  dependent_publishers_ = dependents;
  for (auto dependent : dependent_publishers_) {
    dependent_subscriptions_.push_back(dependent->subscribe());
  }
  report_controller_ = report_controller;
  source_subscription_ = report_controller->subscribe();
  return true;
}

const DataDescription* DataSink::getDataDescription() const {
  return data_description_;
}

size_t DataSink::getTotalNumberOfElements() const {
  return num_total_elements_;
}

size_t DataSink::getNumberOfPaddingElements() const {
  return num_padding_elements_;
}

size_t DataSink::getNumberOfRealElements() const {
  return num_real_elements_;
}

DataSink::~DataSink() {
  if (data_description_) {
    delete data_description_;
  }
  if (source_subscription_) {
    delete source_subscription_;
  }
  for (auto sub : dependent_subscriptions_) {
    delete sub;
  }
  for (auto dependent : dependent_publishers_) {
    delete dependent;
  }
  if (report_controller_) {
    delete report_controller_;
  }
}

} // namespace sim

} // namespace ncs
