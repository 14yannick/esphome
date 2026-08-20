#pragma once

#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"
#include "../hoermann_hcp.h"

namespace esphome::hoermann_hcp {

// Persistent on/off view of a door command the cover has no equivalent for: on commands the door towards the
// position, off closes it again. The door is the ground truth, so a request is only ever a nudge; the switch
// settles on whatever the door actually reports, the same way the cover does.
class HoermannHcpSwitch : public switch_::Switch, public Component {
 public:
  explicit HoermannHcpSwitch(HoermannHcp *parent) : parent_(parent) {}

  void setup() override;
  void dump_config() override;

 protected:
  void write_state(bool state) override;
  void update_from_state_();
  // True once the door is resting in the position this switch represents.
  virtual bool is_on_() const = 0;
  // Commands the door towards that position; returns whatever the door accepted.
  virtual bool turn_on_() = 0;

  HoermannHcp *const parent_;
};

class HoermannHcpVentSwitch final : public HoermannHcpSwitch {
 public:
  using HoermannHcpSwitch::HoermannHcpSwitch;

 protected:
  bool is_on_() const override { return this->parent_->get_door_state() == DoorState::VENT; }
  bool turn_on_() override { return this->parent_->vent_door(); }
};

class HoermannHcpHalfOpenSwitch final : public HoermannHcpSwitch {
 public:
  using HoermannHcpSwitch::HoermannHcpSwitch;

 protected:
  bool is_on_() const override { return this->parent_->get_door_state() == DoorState::HALF_OPEN; }
  bool turn_on_() override { return this->parent_->half_open_door(); }
};

}  // namespace esphome::hoermann_hcp
