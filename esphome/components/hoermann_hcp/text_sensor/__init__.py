import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.types import ConfigType

from .. import CONF_HOERMANN_HCP_ID, HoermannHcp, hoermann_hcp_ns

DEPENDENCIES = ["hoermann_hcp"]

HoermannHcpDoorStateTextSensor = hoermann_hcp_ns.class_(
    "HoermannHcpDoorStateTextSensor", text_sensor.TextSensor, cg.Component
)

CONFIG_SCHEMA = (
    text_sensor.text_sensor_schema(HoermannHcpDoorStateTextSensor)
    .extend({cv.GenerateID(CONF_HOERMANN_HCP_ID): cv.use_id(HoermannHcp)})
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config: ConfigType) -> None:
    parent = await cg.get_variable(config[CONF_HOERMANN_HCP_ID])
    var = await text_sensor.new_text_sensor(config, parent)
    await cg.register_component(var, config)
