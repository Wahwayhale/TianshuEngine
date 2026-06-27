# 天枢引擎 - 脚本系统

## 概述

天枢引擎支持 Lua 脚本，可以通过脚本控制游戏逻辑。

## 设置

### 安装 Lua

```bash
# 通过 FetchContent 自动下载
# 或手动安装 Lua 5.4 + sol2
```

### 初始化脚本引擎

```cpp
ScriptEngine::get().initialize();
```

## 基本用法

### 创建脚本组件

```cpp
auto& entity = scene->createEntity("Player");
auto& script = entity.addComponent<LuaScriptComponent>();
script.scriptPath = "assets/scripts/player.lua";
```

### 脚本文件

```lua
-- player.lua
local speed = 5.0

function OnInit()
    Log.info("Player initialized!")
end

function OnUpdate(deltaTime)
    local pos = entity.position

    if Input.isKeyPressed(Key.W) then
        pos.z = pos.z - speed * deltaTime
    end
    if Input.isKeyPressed(Key.S) then
        pos.z = pos.z + speed * deltaTime
    end

    entity.position = pos
end

function OnDestroy()
    Log.info("Player destroyed!")
end
```

## API 参考

### 实体操作

```lua
-- 获取/设置位置
local pos = entity.position
entity.position = Vec3(1.0, 2.0, 3.0)

-- 获取/设置旋转
local rot = entity.rotation
entity.rotation = Vec3(0.0, 45.0, 0.0)

-- 获取/设置缩放
local scale = entity.scale
entity.scale = Vec3(2.0, 2.0, 2.0)
```

### 输入

```lua
-- 键盘
if Input.isKeyPressed(Key.W) then
    -- W 键按下
end

if Input.isKeyJustPressed(Key.Space) then
    -- 空格键刚按下
end

-- 鼠标
if Input.isMouseButtonPressed(0) then
    -- 鼠标左键按下
end

local x, y = Input.getMousePosition()
```

### 数学

```cpp
-- 向量
local v = Vec3(1.0, 2.0, 3.0)
local len = math.length(v)
local n = math.normalize(v)
local d = math.dot(v1, v2)
local c = math.cross(v1, v2)

-- 插值
local val = math.lerp(a, b, t)
local val = math.clamp(x, min, max)

-- 三角函数
local s = math.sin(angle)
local c = math.cos(angle)
```

### 日志

```lua
Log.info("Information message")
Log.warn("Warning message")
Log.error("Error message")
```

### 时间

```lua
local dt = Time.deltaTime
local total = Time.totalTime
```

## 生命周期

| 函数 | 调用时机 |
|------|----------|
| OnInit | 实体创建时 |
| OnUpdate | 每帧更新 |
| OnDestroy | 实体销毁时 |

## 示例

### 旋转物体

```lua
local speed = 1.0

function OnUpdate(deltaTime)
    local rot = entity.rotation
    rot.y = rot.y + speed * deltaTime * 57.2958
    entity.rotation = rot
end
```

### 弹跳物体

```lua
local amplitude = 1.0
local frequency = 2.0
local startY = 0.0
local time = 0.0

function OnInit()
    startY = entity.position.y
end

function OnUpdate(deltaTime)
    time = time + deltaTime
    local offset = math.sin(time * frequency * 2.0 * math.pi) * amplitude
    local pos = entity.position
    pos.y = startY + math.abs(offset)
    entity.position = pos
end
```

### NPC 巡逻

```lua
local patrolPoints = {
    Vec3(0, 0, 0),
    Vec3(5, 0, 0),
    Vec3(5, 0, 5),
    Vec3(0, 0, 5)
}
local currentPoint = 1
local speed = 2.0

function OnUpdate(deltaTime)
    local target = patrolPoints[currentPoint]
    local pos = entity.position

    local dx = target.x - pos.x
    local dz = target.z - pos.z
    local distance = math.sqrt(dx * dx + dz * dz)

    if distance < 0.1 then
        currentPoint = currentPoint + 1
        if currentPoint > #patrolPoints then
            currentPoint = 1
        end
    else
        local nx = dx / distance
        local nz = dz / distance
        pos.x = pos.x + nx * speed * deltaTime
        pos.z = pos.z + nz * speed * deltaTime
        entity.position = pos
    end
end
```
