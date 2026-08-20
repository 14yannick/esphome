#include <gtest/gtest.h>

#include "esphome/components/hoermann_hcp/switch/hoermann_hcp_switch.h"

#include "../common.h"

namespace esphome::hoermann_hcp::testing {

TEST(HoermannHcpSwitchTest, VentSwitchFollowsTheDoorState) {
  TestableHoermannHcp door;
  HoermannHcpVentSwitch vent(&door);
  vent.setup();
  connect_controller(door);
  ASSERT_FALSE(vent.state);

  // Low byte 0x61 marks the door resting in the vent position.
  door.on_write_registers(BROADCAST_REG, make_registers({0x0000, 0x0000, 0x0061}));
  door.update();
  EXPECT_TRUE(vent.state);

  // Position 200/200, state 0x20 -> resting open, no longer venting.
  door.on_write_registers(BROADCAST_REG, make_registers({0x0000, 0x00C8, 0x2000}));
  door.update();
  EXPECT_FALSE(vent.state);
}

// The intermediate positions are named in the second register, which repeats that name on release.
TEST(HoermannHcpSwitchTest, VentSwitchTurnOnSendsTheVentCommand) {
  TestableHoermannHcp door;
  HoermannHcpVentSwitch vent(&door);
  vent.setup();
  connect_controller(door);

  vent.turn_on();

  auto [pressed, pressed_2] = poll_command(door);
  EXPECT_EQ(pressed, 0x0200);
  EXPECT_EQ(pressed_2, 0x4000);
}

TEST(HoermannHcpSwitchTest, VentSwitchTurnOffClosesADoorThatIsNotClosed) {
  TestableHoermannHcp door;
  HoermannHcpVentSwitch vent(&door);
  vent.setup();
  connect_controller(door);
  // Position 200/200, state 0x20 -> resting open.
  door.on_write_registers(BROADCAST_REG, make_registers({0x0000, 0x00C8, 0x2000}));
  ASSERT_EQ(door.get_door_state(), DoorState::OPEN);

  vent.turn_off();
  EXPECT_EQ(poll_command(door).first, 0x0220);  // COMMAND_CLOSE pressed
}

// The door is already closed, so nothing has to be commanded to reach the off state.
TEST(HoermannHcpSwitchTest, VentSwitchTurnOffDoesNothingWhenAlreadyClosed) {
  TestableHoermannHcp door;  // starts out fully closed
  HoermannHcpVentSwitch vent(&door);
  vent.setup();
  connect_controller(door);

  vent.turn_off();
  EXPECT_EQ(poll_command(door).first, 0x0000);
}

TEST(HoermannHcpSwitchTest, HalfOpenSwitchTurnOnSendsTheHalfOpenCommand) {
  TestableHoermannHcp door;
  HoermannHcpHalfOpenSwitch half_open(&door);
  half_open.setup();
  connect_controller(door);

  half_open.turn_on();

  auto [pressed, pressed_2] = poll_command(door);
  EXPECT_EQ(pressed, 0x0200);
  EXPECT_EQ(pressed_2, 0x0400);
}

TEST(HoermannHcpSwitchTest, HalfOpenSwitchFollowsTheDoorState) {
  TestableHoermannHcp door;
  HoermannHcpHalfOpenSwitch half_open(&door);
  half_open.setup();
  connect_controller(door);

  // Position 100/200, state 0x80 -> resting half open.
  door.on_write_registers(BROADCAST_REG, make_registers({0x0000, 0x0064, 0x8000}));
  door.update();
  EXPECT_TRUE(half_open.state);
}

// A refused command leaves the switch exactly where it was, so unlike the cover (which always republishes its
// position) there is nothing here for a new state to change and thus nothing to publish.
TEST(HoermannHcpSwitchTest, RefusedCommandLeavesTheStateUnpublished) {
  HoermannHcp door;  // never contacted by a bus controller
  HoermannHcpVentSwitch vent(&door);
  vent.setup();
  int publishes = 0;
  vent.add_on_state_callback([&publishes](bool /*state*/) { publishes++; });

  vent.turn_on();

  EXPECT_EQ(poll_command(door).first, 0x0000);
  EXPECT_FALSE(vent.state);
  EXPECT_EQ(publishes, 0);
}

// Nothing is published before the bus controller is heard from, so a door that never reaches the bus would
// otherwise sit at its off default and look healthy.
TEST(HoermannHcpSwitchTest, MissingBusControllerIsFlaggedUntilFirstContact) {
  HoermannHcp door;
  HoermannHcpVentSwitch vent(&door);
  vent.setup();
  EXPECT_TRUE(vent.status_has_warning());

  connect_controller(door);
  door.update();
  EXPECT_FALSE(vent.status_has_warning());
}

}  // namespace esphome::hoermann_hcp::testing
