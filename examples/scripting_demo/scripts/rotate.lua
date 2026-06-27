-- rotate.lua
-- Rotates the entity around the Y axis

local speed = 1.0  -- radians per second

function OnInit()
    Log.info("Rotate script initialized!")
end

function OnUpdate(deltaTime)
    -- Get current rotation
    local rotation = entity.rotation

    -- Rotate around Y axis
    rotation.y = rotation.y + speed * deltaTime * 57.2958  -- Convert to degrees

    -- Apply rotation
    entity.rotation = rotation
end

function OnDestroy()
    Log.info("Rotate script destroyed!")
end
