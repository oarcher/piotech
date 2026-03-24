from esphome import automation
import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_ON_UPDATE, CONF_TRIGGER_ID

CODEOWNERS = ["@oarcher"]

nulluart_ns = cg.esphome_ns.namespace("nulluart")
NullUARTComponent = nulluart_ns.class_(
    "NullUARTComponent", uart.UARTComponent, cg.Component
)
NullUARTOnUpdateTrigger = nulluart_ns.class_(
    "NullUARTOnUpdateTrigger", automation.Trigger.template()
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(NullUARTComponent),
        cv.Optional(CONF_ON_UPDATE): automation.validate_automation(
            {cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(NullUARTOnUpdateTrigger)}
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    for conf in config.get(CONF_ON_UPDATE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)
