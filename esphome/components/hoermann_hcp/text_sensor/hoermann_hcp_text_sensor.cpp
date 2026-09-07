#include "hoermann_hcp_text_sensor.h"

#include "esphome/core/log.h"

namespace esphome::hoermann_hcp {

static const char *const TAG = "hoermann_hcp.text_sensor";

void HoermannHcpDoorStateTextSensor::setup() {
  this->parent_->add_on_state_callback([this]() { this->update_from_state_(); });
  this->update_from_state_();
}

void HoermannHcpDoorStateTextSensor::dump_config() { LOG_TEXT_SENSOR("", "Hoermann HCP Door State", this); }

void HoermannHcpDoorStateTextSensor::update_from_state_() {
  const char *state = door_state_to_string(this->parent_->get_door_state());
  // The door state callback fires on any change on the hub, not just the door state, so this has to dedupe
  // itself rather than relying on publish_state(), which always re-notifies.
  if (this->has_state() && this->get_state() == state)
    return;
  this->publish_state(state);
}

}  // namespace esphome::hoermann_hcp
