#include <gtest/gtest.h>

#include "esphome/components/hoermann_hcp/text_sensor/hoermann_hcp_text_sensor.h"

#include "../common.h"

namespace esphome::hoermann_hcp::testing {

// Nothing has been heard from the bus controller yet, so the sensor starts out seeded with the door's default
// (closed) state.
TEST(HoermannHcpTextSensorTest, StartsAtTheDefaultDoorState) {
  HoermannHcp door;
  HoermannHcpDoorStateTextSensor sensor(&door);
  sensor.setup();
  EXPECT_TRUE(sensor.has_state());
  EXPECT_EQ(sensor.get_state(), "CLOSED");
}

// The reported state follows every door state the bus controller broadcasts.
TEST(HoermannHcpTextSensorTest, FollowsTheDoorState) {
  TestableHoermannHcp door;
  HoermannHcpDoorStateTextSensor sensor(&door);
  sensor.setup();
  connect_controller(door);

  door.on_write_registers(BROADCAST_REG, make_registers({0x0000, 0x0014, 0x0100}));
  door.update();
  EXPECT_EQ(sensor.get_state(), "OPENING");

  door.on_write_registers(BROADCAST_REG, make_registers({0x0000, 0x00C8, 0x2000}));
  door.update();
  EXPECT_EQ(sensor.get_state(), "OPEN");
}

// The hub also changes on position-only broadcasts, which must not re-publish an unchanged door state.
TEST(HoermannHcpTextSensorTest, UnchangedDoorStateIsPublishedOnce) {
  TestableHoermannHcp door;
  HoermannHcpDoorStateTextSensor sensor(&door);
  sensor.setup();
  connect_controller(door);
  int publishes = 0;
  sensor.add_on_state_callback([&publishes](const std::string & /*state*/) { publishes++; });

  door.on_write_registers(BROADCAST_REG, make_registers({0x0000, 0x0014, 0x0100}));
  door.update();
  ASSERT_EQ(publishes, 1);

  // Position moves further along the same OPENING state.
  door.on_write_registers(BROADCAST_REG, make_registers({0x0000, 0x0028, 0x0100}));
  door.update();
  EXPECT_EQ(publishes, 1);
}

}  // namespace esphome::hoermann_hcp::testing
