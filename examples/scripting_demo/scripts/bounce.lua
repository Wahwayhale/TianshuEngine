-- bounce.lua
-- Makes the entity bounce up and down

local amplitude = 1.0   -- Bounce height
local frequency = 2.0   -- Bounces per second
local startY = 0.0      -- Starting Y position
local time = 0.0

function OnInit()
    startY = entity.position.y
    Log.info("Bounce script initialized at Y=" .. tostring(startY))
end

function OnUpdate(deltaTime)
    time = time + deltaTime

    -- Calculate bounce using sine wave
    local offset = math.sin(time * frequency * 2.0 * math.pi) * amplitude

    -- Apply position
    local pos = entity.position
    pos.y = startY + math.abs(offset)  -- Use abs to only bounce upward
    entity.position = pos
end

function OnDestroy()
    Log.info("Bounce script destroyed!")
end
