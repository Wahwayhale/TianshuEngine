#include "ai_code_generator.h"
#include "core/log.h"
#include <ctime>
#include <fstream>

namespace spark {

AICodeGenerator& AICodeGenerator::get() {
    static AICodeGenerator instance;
    return instance;
}

bool AICodeGenerator::initialize() {
    if (m_initialized) return true;

    loadTemplates();
    m_initialized = true;

    SPARK_CORE_INFO("AI Code Generator initialized with {0} templates.", m_templates.size());
    return true;
}

void AICodeGenerator::shutdown() {
    if (!m_initialized) return;
    saveTemplates();
    m_initialized = false;
}

AIResponse AICodeGenerator::generateCode(const std::string& description, const CodeGenOptions& options) {
    std::string systemPrompt;

    if (options.language == "lua") {
        systemPrompt = R"(你是一个游戏开发助手。根据用户的描述，生成可直接使用的 Lua 脚本代码。

要求：
1. 代码必须是完整的、可直接运行的
2. 使用 Spark Engine 的 API
3. 包含 OnInit、OnUpdate、OnDestroy 函数
4. 添加必要的注释
5. 只输出代码，不要其他解释

可用的 API：
- entity.position - 获取/设置位置 (Vec3)
- entity.rotation - 获取/设置旋转 (Vec3)
- entity.scale - 获取/设置缩放 (Vec3)
- entity.velocity - 获取/设置速度 (Vec3)
- Input.isKeyPressed(key) - 检查按键
- Input.isMouseButtonPressed(button) - 检查鼠标按键
- Input.getMousePosition() - 获取鼠标位置
- Time.deltaTime - 帧时间
- Time.totalTime - 总时间
- Log.info(msg) - 输出日志
- Log.warn(msg) - 输出警告
- Log.error(msg) - 输出错误
- math.sin/cos/tan/abs/sqrt - 数学函数
- Vec3(x, y, z) - 创建向量
- math.random() - 随机数

按键常量：
- Key.W, Key.A, Key.S, Key.D - 方向键
- Key.Space - 空格键
- Key.Shift - Shift 键
- Key.Escape - ESC 键)";
    } else if (options.language == "glsl") {
        systemPrompt = R"(你是一个着色器编程助手。根据用户的描述，生成 GLSL 着色器代码。

要求：
1. 使用 GLSL 450 版本
2. 代码必须是完整的、可编译的
3. 添加必要的注释
4. 只输出代码，不要其他解释)";
    } else if (options.language == "cpp") {
        systemPrompt = R"(你是一个 C++ 游戏开发助手。根据用户的描述，生成 C++ 代码。

要求：
1. 使用 C++20 标准
2. 遵循 Spark Engine 的代码风格
3. 添加必要的注释
4. 只输出代码，不要其他解释)";
    }

    if (options.addComments) {
        systemPrompt += "\n请添加详细的中文注释解释代码逻辑。";
    }

    if (options.addErrorHandling) {
        systemPrompt += "\n请添加适当的错误处理。";
    }

    if (options.complexity > 1) {
        systemPrompt += "\n代码复杂度等级: " + std::to_string(options.complexity) + "/5，请相应调整代码复杂度。";
    }

    std::vector<AIMessage> messages = {
        {"system", systemPrompt},
        {"user", description}
    };

    auto response = AIManager::get().chat(messages);

    if (response.success) {
        addToHistory(description, response.content);
    }

    return response;
}

AIResponse AICodeGenerator::generateScript(const std::string& description) {
    CodeGenOptions options;
    options.language = "lua";
    options.style = "game";
    return generateCode(description, options);
}

AIResponse AICodeGenerator::completeCode(const std::string& code, const std::string& context) {
    std::string systemPrompt = R"(你是一个代码补全助手。根据用户提供的代码上下文，补全剩余的代码。

要求：
1. 保持代码风格一致
2. 补全合理的逻辑
3. 只输出需要补全的部分，不要重复已有代码
4. 如果代码已经完整，说明代码已完整)";

    std::string userMessage = "请补全以下代码：\n\n```lua\n" + code + "\n```";

    if (!context.empty()) {
        userMessage += "\n\n上下文信息：" + context;
    }

    std::vector<AIMessage> messages = {
        {"system", systemPrompt},
        {"user", userMessage}
    };

    return AIManager::get().chat(messages);
}

AIResponse AICodeGenerator::explainCode(const std::string& code) {
    std::string systemPrompt = R"(你是一个代码解释助手。用简单易懂的中文解释代码的功能和逻辑。

要求：
1. 解释代码的整体功能
2. 逐行或逐块解释关键逻辑
3. 指出可能的问题或改进建议
4. 使用中文)";

    std::vector<AIMessage> messages = {
        {"system", systemPrompt},
        {"user", "请解释以下代码：\n\n```lua\n" + code + "\n```"}
    };

    return AIManager::get().chat(messages);
}

AIResponse AICodeGenerator::refactorCode(const std::string& code, const std::string& instructions) {
    std::string systemPrompt = R"(你是一个代码重构助手。根据用户的指令重构代码。

要求：
1. 保持代码功能不变
2. 按照用户的指令进行重构
3. 提高代码质量
4. 添加注释说明改动)";

    std::vector<AIMessage> messages = {
        {"system", systemPrompt},
        {"user", "请重构以下代码，要求：" + instructions + "\n\n```lua\n" + code + "\n```"}
    };

    return AIManager::get().chat(messages);
}

AIResponse AICodeGenerator::fixError(const std::string& code, const std::string& error) {
    std::string systemPrompt = R"(你是一个代码调试助手。根据错误信息修复代码中的问题。

要求：
1. 分析错误原因
2. 提供修复后的完整代码
3. 解释修复的原因
4. 避免引入新问题)";

    std::vector<AIMessage> messages = {
        {"system", systemPrompt},
        {"user", "代码有错误，请修复：\n\n错误信息：\n```\n" + error + "\n```\n\n代码：\n```lua\n" + code + "\n```"}
    };

    return AIManager::get().chat(messages);
}

AIResponse AICodeGenerator::optimizeCode(const std::string& code) {
    std::string systemPrompt = R"(你是一个代码优化助手。优化代码的性能和可读性。

要求：
1. 提高代码性能
2. 改善代码可读性
3. 减少不必要的计算
4. 解释优化的原因)";

    std::vector<AIMessage> messages = {
        {"system", systemPrompt},
        {"user", "请优化以下代码：\n\n```lua\n" + code + "\n```"}
    };

    return AIManager::get().chat(messages);
}

AIResponse AICodeGenerator::addComments(const std::string& code) {
    std::string systemPrompt = R"(你是一个代码注释助手。为代码添加详细的中文注释。

要求：
1. 为每个函数添加说明
2. 为关键逻辑添加行内注释
3. 使用中文注释
4. 保持代码原有格式)";

    std::vector<AIMessage> messages = {
        {"system", systemPrompt},
        {"user", "请为以下代码添加注释：\n\n```lua\n" + code + "\n```"}
    };

    return AIManager::get().chat(messages);
}

AIResponse AICodeGenerator::translateCode(const std::string& code, const std::string& fromLang, const std::string& toLang) {
    std::string systemPrompt = R"(你是一个代码翻译助手。将代码从一种语言翻译到另一种语言。

要求：
1. 保持代码功能不变
2. 使用目标语言的最佳实践
3. 添加必要的注释
4. 处理语言差异)";

    std::vector<AIMessage> messages = {
        {"system", systemPrompt},
        {"user", "请将以下 " + fromLang + " 代码翻译成 " + toLang + "：\n\n```" + fromLang + "\n" + code + "\n```"}
    };

    return AIManager::get().chat(messages);
}

std::vector<CodeTemplate> AICodeGenerator::getTemplates() const {
    return m_templates;
}

std::vector<CodeTemplate> AICodeGenerator::getTemplatesByCategory(const std::string& category) const {
    std::vector<CodeTemplate> result;
    for (const auto& templ : m_templates) {
        if (templ.category == category) {
            result.push_back(templ);
        }
    }
    return result;
}

CodeTemplate AICodeGenerator::getTemplate(const std::string& name) const {
    for (const auto& templ : m_templates) {
        if (templ.name == name) {
            return templ;
        }
    }
    return {};
}

void AICodeGenerator::addTemplate(const CodeTemplate& templ) {
    m_templates.push_back(templ);
}

void AICodeGenerator::loadTemplates() {
    // 加载内置模板
    m_templates.clear();

    // 移动脚本模板
    CodeTemplate movement;
    movement.name = "WASD Movement";
    movement.description = "WASD 键控制角色移动";
    movement.category = "Movement";
    movement.code = R"(-- WASD 移动脚本
local speed = 5.0

function OnInit()
    Log.info("Movement script initialized")
end

function OnUpdate(deltaTime)
    local moveDir = Vec3(0, 0, 0)

    if Input.isKeyPressed(Key.W) then
        moveDir.z = moveDir.z - 1
    end
    if Input.isKeyPressed(Key.S) then
        moveDir.z = moveDir.z + 1
    end
    if Input.isKeyPressed(Key.A) then
        moveDir.x = moveDir.x - 1
    end
    if Input.isKeyPressed(Key.D) then
        moveDir.x = moveDir.x + 1
    end

    -- 归一化移动方向
    local length = math.sqrt(moveDir.x * moveDir.x + moveDir.z * moveDir.z)
    if length > 0 then
        moveDir.x = moveDir.x / length
        moveDir.z = moveDir.z / length
    end

    -- 应用移动
    local pos = entity.position
    pos.x = pos.x + moveDir.x * speed * deltaTime
    pos.z = pos.z + moveDir.z * speed * deltaTime
    entity.position = pos
end

function OnDestroy()
    Log.info("Movement script destroyed")
end)";
    m_templates.push_back(movement);

    // 旋转脚本模板
    CodeTemplate rotation;
    rotation.name = "Rotation";
    rotation.description = "物体绕轴旋转";
    rotation.category = "Animation";
    rotation.code = R"(-- 旋转脚本
local speed = 1.0
local axis = Vec3(0, 1, 0)

function OnInit()
    Log.info("Rotation script initialized")
end

function OnUpdate(deltaTime)
    local rotation = entity.rotation
    rotation.x = rotation.x + axis.x * speed * deltaTime * 57.2958
    rotation.y = rotation.y + axis.y * speed * deltaTime * 57.2958
    rotation.z = rotation.z + axis.z * speed * deltaTime * 57.2958
    entity.rotation = rotation
end

function OnDestroy()
    Log.info("Rotation script destroyed")
end)";
    m_templates.push_back(rotation);

    // 弹跳脚本模板
    CodeTemplate bounce;
    bounce.name = "Bounce";
    bounce.description = "物体上下弹跳";
    bounce.category = "Animation";
    bounce.code = R"(-- 弹跳脚本
local amplitude = 1.0
local frequency = 2.0
local startY = 0.0
local time = 0.0

function OnInit()
    startY = entity.position.y
    Log.info("Bounce script initialized at Y=" .. tostring(startY))
end

function OnUpdate(deltaTime)
    time = time + deltaTime

    local offset = math.sin(time * frequency * 2.0 * math.pi) * amplitude

    local pos = entity.position
    pos.y = startY + math.abs(offset)
    entity.position = pos
end

function OnDestroy()
    Log.info("Bounce script destroyed")
end)";
    m_templates.push_back(bounce);

    // NPC 巡逻模板
    CodeTemplate patrol;
    patrol.name = "NPC Patrol";
    patrol.description = "NPC 在多个点之间巡逻";
    patrol.category = "AI";
    patrol.code = R"(-- NPC 巡逻脚本
local patrolPoints = {
    Vec3(0, 0, 0),
    Vec3(5, 0, 0),
    Vec3(5, 0, 5),
    Vec3(0, 0, 5)
}
local currentPoint = 1
local speed = 2.0
local waitTime = 1.0
local currentWait = 0.0

function OnInit()
    Log.info("Patrol script initialized")
end

function OnUpdate(deltaTime)
    local target = patrolPoints[currentPoint]
    local pos = entity.position

    local dx = target.x - pos.x
    local dz = target.z - pos.z
    local distance = math.sqrt(dx * dx + dz * dz)

    if distance < 0.1 then
        -- 到达目标点，等待
        currentWait = currentWait + deltaTime
        if currentWait >= waitTime then
            currentWait = 0.0
            currentPoint = currentPoint + 1
            if currentPoint > #patrolPoints then
                currentPoint = 1
            end
        end
    else
        -- 移动向目标点
        local nx = dx / distance
        local nz = dz / distance

        pos.x = pos.x + nx * speed * deltaTime
        pos.z = pos.z + nz * speed * deltaTime
        entity.position = pos

        -- 朝向移动方向
        local angle = math.atan2(nx, nz)
        local rotation = entity.rotation
        rotation.y = angle * 57.2958
        entity.rotation = rotation
    end
end

function OnDestroy()
    Log.info("Patrol script destroyed")
end)";
    m_templates.push_back(patrol);

    SPARK_CORE_INFO("Loaded {0} code templates.", m_templates.size());
}

void AICodeGenerator::saveTemplates() {
    // 保存自定义模板到文件
    // TODO: 实现模板保存
}

void AICodeGenerator::addToHistory(const std::string& description, const std::string& code) {
    CodeHistory entry;
    entry.description = description;
    entry.generatedCode = code;

    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    entry.timestamp = timestamp;

    m_history.push_back(entry);

    // 限制历史长度
    if (m_history.size() > 100) {
        m_history.erase(m_history.begin());
    }
}

std::vector<AICodeGenerator::CodeHistory> AICodeGenerator::getHistory() const {
    return m_history;
}

void AICodeGenerator::clearHistory() {
    m_history.clear();
}

} // namespace spark
