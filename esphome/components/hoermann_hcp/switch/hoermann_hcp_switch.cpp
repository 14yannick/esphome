#include "hoermann_hcp_switch.h"

#include "esphome/core/log.h"

namespace esphome::hoermann_hcp {

static const char *const TAG = "hoermann_hcp.switch";

void HoermannHcpSwitch::setup() {
  // Nothing is known about the door until the bus controller is heard from, so flag the entity until then.
  this->status_set_warning(LOG_STR("waiting for the bus controller"));
  this->parent_->add_on_state_callback([this]() { this->update_from_state_(); });
}

void HoermannHcpSwitch::dump_config() { LOG_SWITCH("", "Hoermann HCP Switch", this); }

void HoermannHcpSwitch::write_state(bool state) {
  bool accepted = true;
  if (state) {
    if (!this->is_on_())
      accepted = this->turn_on_();
  } else if (this->parent_->get_door_state() != DoorState::CLOSED) {
    accepted = this->parent_->close_door();
  }
  if (!accepted)
    ESP_LOGW(TAG, "Command was not accepted by the door");
  // The door has not moved yet either way, so this republishes the state it is still actually in.
  this->update_from_state_();
}

void HoermannHcpSwitch::update_from_state_() {
  if (!this->parent_->is_valid()) {
    this->status_set_warning(LOG_STR("bus controller not responding"));
    return;
  }
  this->status_clear_warning();
  const bool on = this->is_on_();
  if (on != this->state)
    this->publish_state(on);
}

}  // namespace esphome::hoermann_hcp
