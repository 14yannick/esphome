import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import ICON_AIR_FILTER
from esphome.types import ConfigType

from .. import CONF_HOERMANN_HCP_ID, HoermannHcp, hoermann_hcp_ns

DEPENDENCIES = ["hoermann_hcp"]

CONF_HALF_OPEN = "half_open"
CONF_VENT = "vent"

ICON_GARAGE_OPEN_VARIANT = "mdi:garage-open-variant"

HoermannHcpVentSwitch = hoermann_hcp_ns.class_(
    "HoermannHcpVentSwitch", switch.Switch, cg.Component
)
HoermannHcpHalfOpenSwitch = hoermann_hcp_ns.class_(
    "HoermannHcpHalfOpenSwitch", switch.Switch, cg.Component
)

SWITCH_KEYS = (CONF_VENT, CONF_HALF_OPEN)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_HOERMANN_HCP_ID): cv.use_id(HoermannHcp),
            cv.Optional(CONF_VENT): switch.switch_schema(
                HoermannHcpVentSwitch, icon=ICON_AIR_FILTER
            ).extend(cv.COMPONENT_SCHEMA),
            cv.Optional(CONF_HALF_OPEN): switch.switch_schema(
                HoermannHcpHalfOpenSwitch, icon=ICON_GARAGE_OPEN_VARIANT
            ).extend(cv.COMPONENT_SCHEMA),
        }
    ),
    cv.has_at_least_one_key(*SWITCH_KEYS),
)


async def to_code(config: ConfigType) -> None:
    parent = None
    for key in SWITCH_KEYS:
        if (conf := config.get(key)) is None:
            continue
        if parent is None:
            parent = await cg.get_variable(config[CONF_HOERMANN_HCP_ID])
        var = await switch.new_switch(conf, parent)
        await cg.register_component(var, conf)
