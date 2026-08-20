#include "hoermann_hcp_binary_sensor.h"

#include "esphome/core/log.h"

namespace esphome::hoermann_hcp {

static const char *const TAG = "hoermann_hcp.binary_sensor";

void HoermannHcpConnectedBinarySensor::setup() {
  // Publishing unconditionally is deliberate: the base class dedupes, and filters need every input to drive
  // their timers.
  this->parent_->add_on_state_callback([this]() { this->publish_state(this->parent_->is_valid()); });
  this->publish_initial_state(this->parent_->is_valid());
}

void HoermannHcpConnectedBinarySensor::dump_config() { LOG_BINARY_SENSOR("", "Hoermann HCP Connected", this); }

void HoermannHcpRelayBinarySensor::setup() {
  // Nothing is known about the relay until the bus controller is heard from, so flag the entity until then.
  this->status_set_warning(LOG_STR("waiting for the bus controller"));
  this->parent_->add_on_state_callback([this]() { this->update_from_state_(); });
}

void HoermannHcpRelayBinarySensor::dump_config() { LOG_BINARY_SENSOR("", "Hoermann HCP Relay", this); }

void HoermannHcpRelayBinarySensor::update_from_state_() {
  if (!this->parent_->is_valid()) {
    this->status_set_warning(LOG_STR("bus controller not responding"));
    return;
  }
  if (!this->parent_->is_reg7_known()) {
    this->status_set_warning(LOG_STR("door has not reported the relay"));
    return;
  }
  this->status_clear_warning();
  if (this->parent_->is_relay_on() != this->state)
    this->publish_state(this->parent_->is_relay_on());
}

}  // namespace esphome::hoermann_hcp
